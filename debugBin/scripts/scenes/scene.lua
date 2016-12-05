scene = {}
module("scene", package.seeall)
require "scripts/luaProxy"

luaProxy:new(scene)
function scene:new(o, ud)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	
	if ud ~= nil then
		if type(ud) ~= "userdata" then
			error("argument ud must be a userdata!")
		else
			o.ud = ud
		end
	end

	return o
end

scene.New = 
{
	["function"] = scene.new,
	["argNumber"] = 3,
	["resNumber"] = 1,
}

function scene:update(dt)
	--
end

scene.Update = 
{
	["function"] = scene.update,
	["argNumber"] = 2,	--"self"
	["resNumber"] = 0,
}

scene.sources = {}	-- GameComp array!!! so its elements are lightuserdata
-- every element's key is a name(a string)
-- at first, every element's value is a table containing argument of construcor to create a GameComp
-- after constructor finished, each element's table is discarded, and the pointer replaced it as a userdata
