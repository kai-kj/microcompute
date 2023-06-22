package.path = '../?.lua;' .. package.path
local mc = require("microcompute")(
    "../microcompute_lua.so",
    function (lvl, msg) print(lvl .. ": " .. msg) end
)

local code = [[  
    #version 430
    
    layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
    
    layout(std430, binding = 0) buffer buff1 {
        float data1[5];
    };
    
    layout(std430, binding = 1) buffer buff2 {
        float data2[5];
    };
    
    void main(void) {
        ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
        ivec2 size = ivec2(gl_NumWorkGroups.xy);
        data2[pos.x] = data1[pos.x] * data2[pos.x];
    }
]]

local program = mc.program(code)
if type(program) == "string" then print(program) end

local arr1 = {}
for i = 1, 5 do arr1[i] = math.random() * 100 end
arr1 = mc.struct(arr1)

local arr2 = {}
for i = 1, 5 do arr2[i] = math.random() * 100 end
arr2 = mc.struct(arr2)

print("   " .. tostring(arr1) .. " * " .. tostring(arr2))

local buff1 = mc.buffer()
local buff2 = mc.buffer()

buff1:write(arr1)
buff2:write(arr2)

program:run(mc.vec3(5, 1, 1), buff1, buff2)

buff2:read(arr2)

print(" = " .. tostring(arr2))