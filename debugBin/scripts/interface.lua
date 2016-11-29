--[[
it's a interface that the host program uses to read data from lua scripts
not the game name "Interface" ...
the host program can only get the field(can't be a table) from a global table
so I use this lua script as a interface
and you should NOT overwrite any of them unless you can do it better
--]]

-- all lua functions are registered in this table as a unique name like "enemy_wolf_attack_1" or "table1|table2|table3|foo"?
-- so that when the game needs to call it,
-- the game will find it in this table with the unique name quickly ang call it
LuaFunctions = {} --global

Interface = {}  --global

-- restore the files already were loaded as function by name of file's path
-- so if the readData function do a file, it'll check this table first
-- if the file to be done is already in this table, it'll call the function restored in this table
-- so it's not wise to edit script files during the game running
Interface.loadedFiles = {}

-- the host program will call readData every time when it needs to read field from the file
-- so if the argument "file" == Interface.doingFile, readData won't do the file, and only read data from the global table
-- but if you want to do the file often, you can assign Interface.doingFile to nil when you need to dofile
-- so, the new script's global varieties will overwrite the old ones
Interface.doingFile = nil

-- field: "table1|table2|table3|field&table4|field2" will return two result
-- so the table's name or field's name can't have '&' or '|'
function Interface.readData(file, field)
    if type(file) ~= "string" or type(field) ~= "string" then
        error("readData's two arguments must be string!", 1)
    end

    local f = Interface.loadedFiles[file]
    if f == nil then
        f, err = loadfile(file)
        --temp
        --print("load once file: " .. file)
        if f == nil then
            error("Cannot load script: \"" .. file .."\": " .. err, 1)
        else
            Interface.loadedFiles[file] = f
        end
    end

    if doingFile ~= file then
        -- do the script file
        f()
        --temp
        --print("do once file: " .. file)
        doingFile = file
    end

    -- read data
    local length = 0
    local res = {}
    --local fieldsList = {}
    for singleField in string.gmatch(field, "[^&]+") do
        --fieldsList[#fieldsList + 1] = {}
        length = length + 1
        local currentValue = _G
        for fieldPath in string.gmatch(singleField, "[^|]+") do
            --fieldsList[length][#(fieldsList[length]) + 1] = fieldPath
            if type(currentValue) == "table" then
                currentValue = currentValue[fieldPath]
            else
                break
            end
        end
        res[length] = currentValue
        --restore funcitons
        if(type(currentValue) == "function") then
            LuaFunctions[singleField] = currentValue
        end
    end
--[[
    --temp
    print(unpack(res))
    for i = 1, #fieldsList do
        print(unpack(fieldsList[i]))
    end
--]]
    return unpack(res)
end

--[[
--test
print(Interface.readData("config.lua", "Config|window|width&Config|FPS&Config|tileWidth"))
print(Interface.readData("script.lua", "s&t&a&b&foo"))
for _, v in pairs(LuaFunctions) do
    print(v)
end
print(Interface.readData("config.lua", "Config|window|caption&Config|FPS"))
--]]
