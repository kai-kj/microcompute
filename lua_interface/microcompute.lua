assert(type(jit) == "table", "use luajit to use this library")

local math = require("math")
local ffi = require("ffi")

--- @version JIT
--- @class mc
local mc = {}

--- @class cfuncs
--- @field databuff_create function
--- @field databuff_read function
--- @field databuff_write function
--- @field databuff_destroy function
--- @field lmc_initialize function
--- @field mc_finish_tasks function
--- @field mc_get_time function
--- @field mc_buffer_create function
--- @field mc_buffer_destroy function
--- @field mc_buffer_set_size function
--- @field lmc_buffer_write function
--- @field lmc_buffer_read function
--- @field mc_program_create function
--- @field mc_program_destroy function
--- @field mc_program_check function
--- @field lmc_program_run function
local cfuncs = {}

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

--- Create a vec2.
--- @param ... number The values of the vector, leave empty for 0s
--- @return vector
function mc.vec2(...) return vector.new(2, ...) end

--- Create a vec3.
--- @param ... number The values of the vector, leave empty for 0s
--- @return vector
function mc.vec3(...) return vector.new(3, ...) end

--- Create a vec4.
--- @param ... number The values of the vector, leave empty for 0s
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
    for i = 1, self.size do self[i] = #args == 0 and 0 or args[i] end
    return self
end

--- Returs a table only containing the values of the vector.
--- @return table
function vector:value()
    local value = {}
    for i, k in ipairs(self) do value[i] = k end
    return value
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
function vector:pack_into(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        cfuncs.databuff_write(buff, i == 1 and align or 1, self[i])
    end
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
function vector:unpack_from(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        self[i] = cfuncs.databuff_read(buff, i == 1 and align or 1)
    end
end

--- Get the string representation of a vector.
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

--- Create a new struct. Can also be used to represent arrays.
--- @param values table The values of the table, packed by index order
--- @return struct
function mc.struct(values) return struct.new(values) end

--- @private
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

--- Returs a table only containing the values of the table.
--- @return table
function struct:value()
    local value = {}
    for i, k in ipairs(self) do value[i] = k end
    return value
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
function struct:pack_into(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        if type(self[i]) == "number" then
            cfuncs.databuff_write(buff, align, self[i])
        else
            self[i]:pack_into(buff, align)
        end
    end
end

--- @package
--- @param buff ffi.cdata*
--- @param align? integer
function struct:unpack_from(buff, align)
    align = math.max(self.align, align and align or self.align)
    for i = 1, #self do
        if type(self[i]) == "number" then
            self[i] = cfuncs.databuff_read(buff, align)
        else
            self[i]:unpack_from(buff, align)
        end
    end
end

--- Get the string representation of a table.
--- @return string
function struct:__tostring()
    local str = "{ "
    for i = 1, #self do str = str .. tostring(self[i]) .. " " end
    return str .. "}"
end

--- Wait for all compute processes to finish, and return the time spent waiting,
--- in seconds.
--- @return number
function mc.finishtasks()
    return cfuncs.mc_finish_tasks()
end

--- Get the current time, in seconds.
--- @return number
function mc.gettime()
    return cfuncs.mc_get_time()
end

--- @class buffer
--- @field private size integer
--- @field package mcbuff ffi.cdata*
local buffer = {}

buffer.__index = buffer

--- Create a buffer.
--- @param size integer The size of the buffer.
--- @return buffer
function mc.buffer(size) return buffer.new(size) end

--- @param v any
--- @return boolean
local function isbuffer(v) return getmetatable(v) == buffer end

--- @private
--- @param size integer
--- @return buffer
function buffer.new(size)
    local self = {}
    setmetatable(self, buffer)

    self.size = size
    self.mcbuff = cfuncs.mc_buffer_create(self.size and self.size * 4 or 0)
    ffi.gc(self.mcbuff, cfuncs.mc_buffer_destroy)
    return self
end

--- Change the size of a buffer.
--- @param size integer The new size of the buffer
function buffer:setsize(size)
    self.size = size
    cfuncs.mc_buffer_set_size(self.mcbuff, self.size * 4)
end

--- Write to a buffer.
--- @param data vector|struct The value to write to the buffer
function buffer:write(data)
    assert(data.size <= self.size, "data too large")

    local databuff = cfuncs.databuff_create(data.size)
    ffi.gc(databuff, cfuncs.databuff_destroy)
    data:pack_into(databuff)
    cfuncs.lmc_buffer_write(self.mcbuff, databuff)
end

--- Read a value from a buffer.
--- @param data vector|struct The value to read the buffer into
function buffer:read(data)
    assert(data.size <= self.size, "data too large")

    local databuff = cfuncs.databuff_create(data.size)
    ffi.gc(databuff, cfuncs.databuff_destroy)
    cfuncs.lmc_buffer_read(self.mcbuff, databuff)
    data:unpack_from(databuff)
end

--- @class program
--- @field ptr ffi.cdata*
local program = {}

program.__index = program

--- Create a program. Retunrs a string containing an error message on failure.
--- @param data string The GLSL code for the program
--- @return program|string
function mc.program(data) return program.new(data) end

--- @private
--- @param code string
--- @return program|string
function program.new(code)
    local self = {}
    setmetatable(self, program)

    self.ptr = cfuncs.mc_program_create(code)
    ffi.gc(self.ptr, function(ptr) cfuncs.mc_program_destroy(ptr) end)
    local res = cfuncs.mc_program_check(self.ptr)

    return res ~= nil and ffi.string(res):sub(1, -2) or self
end

--- Run a program. BUffers are assigned a binding (starting at 0) based on the
--- order that they are passed into this function.
--- @param size vector The distpatch size of the program
--- @param ... buffer  Buffers to pass to the program
function program:run(size, ...)
    assert(size.size == 3, "size must be vec3")

    local buffs = {}
    for i, v in ipairs({ ... }) do buffs[i] = v.mcbuff end
    buffs[#buffs + 1] = ffi.new("void *", nil)

    cfuncs.lmc_program_run(
        self.ptr, size[1], size[2], size[3], unpack(buffs)
    )
end

--- Initialize the microcompute library.
---
--- The callback function should take two
--- arguments, level and message, where level is one of "info", "low", "medium",
--- "high", and message is the error message.
---
--- Returns the mc libarary on success, and a string containing an error message
--- on failure.
---
--- @param libpath string The path to microcompute_lua.so
--- @param cb function    A function to call when errors occur
--- @return mc|string
return function(libpath, cb)
    cfuncs = ffi.load(libpath)
    ffi.cdef [[
        void* databuff_create(int size);
        float databuff_read(void* databuff, int align) ;
        void databuff_write(void* databuff, int align, float value);
        void databuff_destroy(void* databuff);
        char* lmc_initialize(void(cb)(int level, const char* msg));
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

    local cbwrap = function(lvl, msg)
        local t = { "info", "low", "medium", "high" }
        cb(t[lvl + 1], ffi.string(msg))
    end

    local ret = ffi.string(cfuncs.lmc_initialize(cbwrap))
    return ret == "success" and mc or ret
end
