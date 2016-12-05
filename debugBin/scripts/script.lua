Script = {}

print"hello lua module!"

s = "it's a lua string"

t = true

a = 100

b = 2.3

tab = {1, 2, "a string", true}

funcs = {}
function funcs.foo()
    return
        function(a)
            return a * 2
        end,
        1,
        1
end

foo2 = {}
foo2["function"] = function(a, t, b)
	print(a .. b)
    print(unpack(t))
	return 100
end
foo2["argNumber"] = 3
foo2["resNumber"] = 1

funcs["foo2"] = foo2

--ud = 0
--lud = 0

t1 = {}
t1.t2 = {}
t1.t2.t3 = {}
t1.t2.t3.v = 100
t1.t2.t3.a = 200
t1.t2.t3.foo = function()
    return
        function(a)
            print("succeed!!!")
            x, y = CFunctions.test(a)
            return
                1.5,
                x, y,
                nil,
                {},
                function() return 1 end,
                "a lua string returned by lua function.",
                true,
                ud,
                lud
        end,
        1,
        8
end

