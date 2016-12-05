startMenu = {}
module("startMenu", package.seeall)
require "./scripts/scenes/scene"

scene:new(startMenu)
function startMenu:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

