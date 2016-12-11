canvas = {}
module("canvas", package.seeall)
require "luaProxy"
require "tools"

luaProxy:new(canvas)
function canvas:new(o, ud)
	o = o or {}
	setmetatable(o, self)
	self.__index = function(table, key)
		local value = self[key]
		table[key] = tools.deepCopy(value)
		return table[key]
	end
	
	if ud ~= nil then
		if type(ud) ~= "userdata" then
			error("argument ud must be a userdata!")
		else
			o.ud = ud
		end
	end

	return o
end

canvas.New = 
{
	["function"] = canvas.new,
	["argNumber"] = 3,
	["resNumber"] = 1,
}