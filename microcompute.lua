local math = require("math")
local ffi = require("ffi")

--- @class mc
--- @field package cfuncs table
local mc = {}

--- @param curr integer
--- @param align integer
--- @return integer
local function calc_offset(curr, align)
    return (align - curr % align) % align
end

--- @class vector
--- @field public size integer
--- @field private align integer
local vector = {}

vector.__index = vector

--- @param v any
--- @return boolean
local function isvector(v) return getmetatable(v) == vector end

--- @param ... number
--- @return vector
function mc.vec2(...) return vector.new(2, ...) end

--- @param ... number
--- @return vector
function mc.vec3(...) return vector.new(3, ...) end

--- @param ... number
--- @return vector
function mc.vec4(...) return vector.new(4, ...) end

--- @private
--- @param size integer
--- @param ... number
--- @return vector
function vector.new(size, ...)
    local args = { ... }
    assert(#args == 0 or #args == size, "not enough elements supplied")

    local self = {}
    setmetatable(self, vector)

    self.size = size
    self.align = self.size == 2 and 2 or 4
    for i = 1, self.size do
        if #args == 0 then self[i] = 0
        else self[i] = args[i] end
    end
    return self
end

--- @return table
function vector:value()
    local value = {}
    for i, k in ipairs(self) do value[i] = k end
    return value
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
--- @return nil
function vector:pack_into(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        mc.cfuncs.databuff_write(buff, i == 1 and align or 1, self[i])
    end
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
--- @return nil
function vector:unpack_from(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        self[i] = mc.cfuncs.databuff_read(buff, i == 1 and align or 1)
    end
end

--- @return string
function vector:__tostring()
    local str = "( "
    for i = 1, #self do str = str .. self[i] .. " " end
    return str .. ")"
end

--- @class struct
--- @field public size integer
--- @field private align integer
local struct = {}

struct.__index = struct

--- @param v any
--- @return boolean
local function isstruct(v) return getmetatable(v) == struct end

--- @param values table
--- @return struct
function mc.struct(values) return struct.new(values) end

--- @param values table
--- @return struct
function struct.new(values)
    local self = {}
    setmetatable(self, struct)

    self.size = 0
    self.align = 1
    for i = 1, #values do
        self[i] = values[i]
        if type(self[i]) == "number" then
            self.size = self.size + 1
        elseif isvector(self[i]) or isstruct(self[i]) then
            self.align = math.max(self.align, self[i].align)
            self.size = self.size
                      + calc_offset(self.size, self[i].align)
                      + self[i].size
        else
            error("values must be number, vector, or struct")
        end
    end
    return self
end

--- @return table
function struct:value()
    local value = {}
    for i, k in ipairs(self) do value[i] = k end
    return value
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
--- @return nil
function struct:pack_into(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        if type(self[i]) == "number" then
            mc.cfuncs.databuff_write(buff, align, self[i])
        else
            self[i]:pack_into(buff, align)
        end
    end
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
--- @return nil
function struct:unpack_from(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        if type(self[i]) == "number" then
            self[i] = mc.cfuncs.databuff_read(buff, align)
        else
            self[i]:unpack_from(buff, align)
        end
    end
end

--- @return string
function struct:__tostring()
    local str = "{ "
    for i = 1, #self do str = str .. tostring(self[i]) .. " " end
    return str .. "}"
end

--- @return nil
function mc.terminate()
    mc.cfuncs.mc_terminate()
end

--- @return nil
function mc.finishtasks()
    mc.cfuncs.mc_finish_tasks()
end

--- @return number
function mc.gettime()
    return mc.cfuncs.mc_get_time()
end

--- @class buffer
--- @field private size integer
--- @field package mcbuff ffi.cdata*
local buffer = {}

buffer.__index = buffer

--- @param size integer
--- @return buffer
function mc.buffer(size) return buffer.new(size) end

--- @param v any
--- @return boolean
local function isbuffer(v) return getmetatable(v) == buffer end

--- @param size integer
--- @return buffer
function buffer.new(size)
    local self = {}
    setmetatable(self, buffer)

    self.size = size
    self.mcbuff = mc.cfuncs.mc_buffer_create(self.size and self.size * 4 or 0)
    ffi.gc(self.mcbuff, mc.cfuncs.mc_buffer_destroy)
    return self
end

--- @param size integer
--- @return nil
function buffer:setsize(size)
    self.size = size
    mc.cfuncs.mc_buffer_set_size(self.mcbuff, self.size * 4)
end

--- @param data vector|struct
--- @return nil
function buffer:write(data)
    assert(data.size <= self.size, "data too large")

    local databuff = mc.cfuncs.databuff_create(data.size)
    ffi.gc(databuff, mc.cfuncs.databuff_destroy)
    data:pack_into(databuff)
    mc.cfuncs.lmc_buffer_write(self.mcbuff, databuff)
end

--- @param data vector|struct
--- @return nil
function buffer:read(data)
    assert(data.size <= self.size, "data too large")

    local databuff = mc.cfuncs.databuff_create(data.size)
    ffi.gc(databuff, mc.cfuncs.databuff_destroy)
    mc.cfuncs.lmc_buffer_read(self.mcbuff, databuff)
    data:unpack_from(databuff)
end

--- @class program
--- @field ptr ffi.cdata*
local program = {}

program.__index = program

--- @param data string
--- @return program|string
function mc.program(data) return program.new(data) end

--- @param code string
--- @return program|string
function program.new(code)
    local self = {}
    setmetatable(self, program)

    self.ptr = mc.cfuncs.mc_program_create(code)
    ffi.gc(self.ptr, function(ptr) mc.cfuncs.mc_program_destroy(ptr) end)
    local res = mc.cfuncs.mc_program_check(self.ptr)

    if mc.cfuncs.is_null(res) ~= 1 then return ffi.string(res):sub(1, -2)
    else return self end
end

--- @param size vector
--- @param ... buffer
--- @return nil
function program:run(size, ...)
    assert(size.size == 3, "size must be vec3")

    local buffs = {}
    for i, v in ipairs({...}) do buffs[i] = v.mcbuff end
    buffs[#buffs + 1] = mc.cfuncs.make_null()

    mc.cfuncs.lmc_program_run(
        self.ptr, size[1], size[2], size[3], unpack(buffs)
    )
end

--- @param libpath string
--- @param cb function
--- @return mc|string
return function (libpath, cb)
    mc.cfuncs = ffi.load(libpath)
    ffi.cdef[[
        void free(void *ptr);
        int is_null(void *ptr);
        void* make_null();

        void* databuff_create(int size);
        float databuff_read(void* databuff, int align) ;
        void databuff_write(void* databuff, int align, float value);
        void databuff_destroy(void* databuff);

        char* lmc_initialize(void(cb)(int level, const char* msg));
        void mc_terminate();
        double mc_finish_tasks();
        double mc_get_time();
        
        void* mc_buffer_create(unsigned long size);
        void mc_buffer_destroy(void* buffer);
        void mc_buffer_set_size(void* buffer, unsigned long size);
        void lmc_buffer_write(void* buffer, void* databuff);
        void lmc_buffer_read(void* buffer, void* databuff);
        
        void* mc_program_create(const char* code);
        void mc_program_destroy(void* program);
        char* mc_program_check(void* program);
        void lmc_program_run(void* program, int x, int y, int z, ...);
    ]]

    local cbwrap = function (lvl, msg)
        local t = {"info", "low", "medium", "high"}
        cb(t[lvl + 1], ffi.string(msg))
    end

    local ret = ffi.string(mc.cfuncs.lmc_initialize(cbwrap))
    if ret == "success" then return mc else return ret end
end