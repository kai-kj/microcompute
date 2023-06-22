local math = require("math")
local ffi = require("ffi")

local mc = {}

local function isnumber(v) return type(v) == "number" end
local function istable(v) return type(v) == "table" end

local function calc_offset(curr, align)
    return (align - curr % align) % align
end

local vector = {}
vector.__index = vector
local function isvector(v) return getmetatable(v) == vector end

function mc.vec2(x, y) return vector.new(2, x, y) end
function mc.vec3(x, y, z) return vector.new(3, x, y, z) end
function mc.vec4(x, y, z, w) return vector.new(4, x, y, z, w) end

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
        assert(isnumber(self[i]), "values must be numbers")
    end
    return self
end

function vector:pack_into(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        mc._cfuncs.databuff_write(buff, i == 1 and align or 1, self[i])
    end
end

function vector:unpack_from(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        self[i] = mc._cfuncs.databuff_read(buff, i == 1 and align or 1)
    end
end

function vector:__tostring()
    local str = "( "
    for i = 1, #self do str = str .. self[i] .. " " end
    return str .. ")"
end

local struct = {}
struct.__index = struct
local function isstruct(v) return getmetatable(v) == struct end

function mc.struct(values) return struct.new(values) end

function struct.new(values)
    assert(istable(values), "values must be table")

    local self = {}
    setmetatable(self, struct)

    self.size = 0
    self.align = 1
    for i = 1, #values do
        self[i] = values[i]
        if isnumber(self[i]) then
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

function struct:pack_into(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        if isnumber(self[i]) then mc._cfuncs.databuff_write(buff, align, self[i])
        else self[i]:pack_into(buff, align) end
    end
end

function struct:unpack_from(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        if isnumber(self[i]) then self[i] = mc._cfuncs.databuff_read(buff, align)
        else self[i]:unpack_from(buff, align) end
    end
end

function struct:__tostring()
    local str = "{ "
    for i = 1, #self do str = str .. tostring(self[i]) .. " " end
    return str .. "}"
end

function mc.terminate()
    mc._cfuncs.mc_terminate()
    mc.initialized = false
end

function mc.finishtasks()
    mc._cfuncs.mc_finish_tasks()
end

function mc.gettime()
    mc._cfuncs.mc_get_time()
end

local buffer = {}
buffer.__index = buffer
setmetatable(buffer, { __call = buffer.new })

function mc.buffer() return buffer.new() end
local function isbuffer(v) return getmetatable(v) == buffer end

function buffer.new()
    local self = {}
    setmetatable(self, buffer)
    self.mcbuff = mc._cfuncs.mc_buffer_create(0)
    ffi.gc(self.mcbuff, mc._cfuncs.mc_buffer_destroy)
    return self
end

function buffer:write(data)
    assert(isvector(data) or istable(vector), "data must be vector or table")

    local databuff = mc._cfuncs.databuff_create(data.size)
    ffi.gc(databuff, mc._cfuncs.databuff_destroy)
    data:pack_into(databuff)
    mc._cfuncs.lmc_buffer_write(self.mcbuff, databuff)
end

function buffer:read(data)
    assert(isvector(data) or istable(vector), "data must be vector or table")

    local databuff = mc._cfuncs.databuff_create(data.size)
    ffi.gc(databuff, mc._cfuncs.databuff_destroy)
    mc._cfuncs.lmc_buffer_read(self.mcbuff, databuff)
    data:unpack_from(databuff)
end

local program = {}
program.__index = program

function mc.program(data) return program.new(data) end

function program.new(code)
    assert(type(code) == "string", "code must be a string")

    local self = {}
    setmetatable(self, program)

    self.ptr = mc._cfuncs.mc_program_create(code)
    ffi.gc(self.ptr, function(ptr) mc._cfuncs.mc_program_destroy(ptr) end)
    local res = mc._cfuncs.mc_program_check(self.ptr)

    if mc._cfuncs.is_null(res) ~= 1 then return ffi.string(res):sub(1, -2)
    else return self end
end

function program:run(size, ...)
    assert(isvector(size) and size.size == 3, "size must be vec3")

    local buffs = {}
    for i, v in ipairs({...}) do
        assert(isbuffer(v), "... must be buffers")
        buffs[i] = v.mcbuff
    end
    buffs[#buffs + 1] = mc._cfuncs.make_null()
    
    mc._cfuncs.lmc_program_run(self.ptr, size[1], size[2], size[3], unpack(buffs))
end

return function (libpath, cb)
    assert(type(libpath) == "string", "libpath must be string")
    assert(type(cb) == "function", "cb must be a function")

    mc._cfuncs = ffi.load(libpath)
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

    local ret = ffi.string(mc._cfuncs.lmc_initialize(cbwrap))
    if ret == "success" then return mc else return ret end
end