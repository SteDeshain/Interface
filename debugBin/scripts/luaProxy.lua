--该文件表示了一个luaProxy必须的最基本的元素，要写一个luaProxy必须按照该文件规则，要拥有该文件所有元素!
luaProxy = {}
module("luaProxy", package.seeall)
--require "fatherClass"
require "tools"

--fatherClass:new(luaProxy)

luaProxy.__ref = true

--每当在c++中new一个luaProxy，会在lua全局变量中注册一个唯一名字string作为key的table，作为该对象在lua中的代理
--并在new函数中，把c++对象的地址作为lightuserdata储存在代理table的ud字段中，以便代理的调用
--该唯一名字的规则是：
--	必须为luaProxy_xxx的形式，其中luaProxy为其具体的类型名称，如welcomeScene等，其中xxx为不是数字的字符，所以类型名中不允许有下划线
--	若c++传入的名称即为luaProxy_xxx的形式，即可直接沿用，但先在lua全局变量中寻找是否已存在相同的名字
--		若已存在同名的变量，则在原有的luaProxy_xxx基础上加上"_1"的数字编号，若该编号仍然存在，则编号加一，以此类推，直到不重名为止
--	若c++传入的名称luaProxy_2的形式，即为上一种情况后缀为数字的情况，则先把数字的前导0除去(如果有的话)
--		然后再查找重名，若有重名，则使后缀数字+1之后再查找重名，直到不重名为止
--	若c++传入的名称为luaProxy，没有任何后缀，则先自动生成数字后缀"_1"加到名称后面，之后按照上面一条的情况查找重名
function luaProxy:new(o, ud)
	local o = o or {}
	setmetatable(o, self)
	self.__index = function(table, key)
		local value = self[key]
		table[key] = tools.deepCopy(value)
		return table[key]
	end
	
	--因此该new函数即可作为新建对象时使用，也可在子类中作为继承语句用
	if ud ~= nil then
		if type(ud) ~= "userdata" then
			error("argument ud must be a userdata!")
		else
			o.ud = ud
		end
	end
	
	return o
end

--在c++中调用的函数皆以大写开头，lua中调用的函数以小写开头，以避免冲突
luaProxy.New = 
{
	["function"] = luaProxy.new,
	["argNumber"] = 3,	--remember "self"
	["resNumber"] = 1,
}