package.path = "../../lua_interface/?.lua;" .. package.path

local code = [[
    #version 430

    layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

    layout(std430, binding = 0) buffer dBuff {
        float maxIter;
        vec2 center;
        float zoom;
    };

    layout(std430, binding = 1) buffer iBuff {
        float img[];
    };

    void main(void) {
        ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
        ivec2 size = ivec2(gl_NumWorkGroups.xy);
        vec2 screenPos = vec2(pos) / vec2(size) - 0.5;
        vec2 z0 = center + screenPos / zoom;
        vec2 z = vec2(0.0, 0.0);
        vec2 z2 = vec2(0.0, 0.0);
        int i = 0;
        for(; i < maxIter && z2.x + z2.y <= 4; i++) {
            z = vec2(z2.x - z2.y + z0.x, 2 * z.x * z.y + z0.y);
            z2 = vec2(z.x * z.x, z.y * z.y);
        }
        img[pos.y * size.x + pos.x] = float(i) / float(maxIter);
    }
]]

local mc = require("microcompute")(
    "../../lua_interface/microcompute_lua.so",
    function(lvl, msg) print(lvl .. ": " .. msg) end
)

local size = mc.vec3(1000, 1000, 1);
local center = mc.vec2(-0.7615, -0.08459);
local zoom = 1000;
local maxIter = 1000;

local program = mc.program(code)
if type(program) == "string" then
    print(program)
    return
end

local settings = mc.struct { maxIter, center, zoom }

local img = {}
for i = 1, size[1] * size[2] do img[i] = 0 end
img = mc.struct(img)

local dbuff = mc.buffer(settings.size)
local ibuff = mc.buffer(img.size)

dbuff:write(settings)

program:run(size, dbuff, ibuff)

ibuff:read(img)

local file = io.open("mandelbrot.ppm", "w")

file:write("P3 " .. size[1] .. " " .. size[2] .. " 255\n")

for i = 1, size[2] do
    for j = 1, size[1] do
        local val = math.floor(img[(i - 1) * size[1] + j] * 255)
        file:write(val .. " " .. val .. " " .. val .. "\n")
    end
end

file:close()
