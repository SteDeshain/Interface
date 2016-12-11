welcomeScene = {}
module("welcomeScene", package.seeall)
require "scenes/scene"
require "tools"

scene:new(welcomeScene)
function welcomeScene:new(o, ud)
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

welcomeScene.New = 
{
	["function"] = welcomeScene.new,
	["argNumber"] = 3,
	["resNumber"] = 1,
}

--temp
welcomeScene.n = 0
welcomeScene.ct = 1

function welcomeScene:update(dt)
	--...
	self.n = self.n + dt
	if self.ct == 1 then
		if self.n >= 2000 then
			print("2 seconds..." .. self.n)
			--self.n = 0
			CFunctions.showCanvas(self.sources.logoCanv_1.ud)
			self.ct = self.ct + 1
		end
	elseif self.ct == 2 then
		if self.n >= 4000 then
			print("2 seconds..." .. self.n)
			--self.n = 0
			CFunctions.hideCanvas(self.sources.logoCanv_1.ud)
			self.ct = self.ct + 1
		end
	elseif self.ct ==3 then
		if self.n >= 6000 then
			print("2 seconds..." .. self.n)
			--self.n = 0
			CFunctions.showCanvas(self.sources.logoCanv_2.ud)
			self.ct = self.ct + 1
		end
	elseif self.ct == 4 then
		if self.n >= 8000 then
			print("2 seconds..." .. self.n)
			self.n = 0
			CFunctions.hideCanvas(self.sources.logoCanv_2.ud)
			self.ct = 1
		end
	end
end

welcomeScene.Update = 
{
	["function"] = welcomeScene.update,
	["argNumber"] = 2,
	["resNumber"] = 0,
}

-- 英文实在憋不出来了
-- "length"有助于c++代码处理循环次数
-- 每一项中都有"name"字段，在该项构建后，会在本sources表中保存一个以相应name为key的字段，
-- 对应value为一个luaProxy的table(其实该luaProxy已在全局变量中注册过，不过全局的key的名字可能和sources中的不一样)，来存放相应的对象（也可以考虑连同描述class的字符串一并存进去???）
-- 因此各项的顺序尤为重要，以防止后面的项在构建时找不到相应依赖的userdata
-- 在一项构建完成后，可删除相应参数表
-- 在构建每一项时，根据"class"字段判断对象的类，以调用正确的构造函数，但在c++代码中，每一个变量都是GameComp*，但由于多态的便捷，会将不同类型的GameComp储存到正确的list中去
welcomeScene.sources = {}
welcomeScene.sources["length"] = 4

welcomeScene.sources.logoCanv_1 = {}
welcomeScene.sources[1] =
{
	-- name registered in this table and global, and also is the script name
	["name"] = "logoCanv_1",	--脚本的编写者只负责在sources表中的命名不冲突，这对脚本编写者来说足够了
								--但在全局变量中，相应的luaProxy的key很有可能和sources表中的key不同，因为在这里该命名可能不冲突，但放在全局中可能和其他的luaProxy冲突
								--所以在c++代码中，会处理以防全局中的命名冲突
								--如果为nil，则不会把该对象注册在lua全局变量中，不会创建相应luaProxy
	["class"] = "Canvas",
	[1] = "color",					-- color mode
	[2] = {0, 128, 255, 255},		-- color: white
	[3] = 0.8,						-- transparency: 0.0f
---[[
	[4] = {-100, -100, Config.window.width, Config.window.height},
									-- viewRect: whole window
	[5] = {Config.window.width, Config.window.height},
									-- canvasSize: whole window
--]]
	[6] = nil,						-- motherCanvas: NULL
	[7] = false,					-- startVisible
}
welcomeScene.sources.gui_1 = {}
welcomeScene.sources[2] =
{
	["name"] = "gui_1",
	["class"] = "GUI",
	[1] = "picture",					-- mode, not used in constructor, used in c++ to decide to use which constructor
	[2] = Config.window.width / 2,		-- x
	[3] = Config.window.height / 2,		-- y
	[4] = 1,							-- drawIndex
	--[5] = nil,
	[5] = ".sources.logoCanv_1.ud",		-- motherCanvas
										-- 以点开头，表示先以该GameComp的motherScene为起始查找为止
										-- 否则，在全局变量中开始查找
	[6] = 1,							-- textureNum
	[7] = "sources/textures/sheet.png",	-- fileName
}

welcomeScene.sources.logoCanv_2 = {}
welcomeScene.sources[3] =
{
	["name"] = "logoCanv_2",
	["class"] = "Canvas",
	[1] = "color",
	[2] = {0, 0, 250, 255},
	--[1] = "picture",
	--[2] = "sources/textures/tiles.png",
	[3] = 0.5,
	[4] = {100, 100, 400, 200},
	[5] = {512, 256},
	[6] = nil,
	[7] = false,
}
welcomeScene.sources.gui_2 = {}
welcomeScene.sources[4] =
{
	["name"] = "gui_2",
	["class"] = "GUI",
	[1] = "color",
	[2] = 100,
	[3] = 100,
	[4] = 1,
	[5] = ".sources.logoCanv_2.ud",
	[6] = {100, 100},								-- picSize
	[7] = {255, 0, 0, 0},						-- color
	[8] = 0.8,									-- transparency
}
