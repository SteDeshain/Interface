logoCanv = {}
module("logoCanv", package.seeall)
require "comps/canvas"
require "tools"

canvas:new(logoCanv)
function logoCanv:new(o, ud)
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

logoCanv.New = 
{
	["function"] = logoCanv.new,
	["argNumber"] = 3,
	["resNumber"] = 1,
}