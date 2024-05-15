return {
    groupName = "isfunction",
    cases = {
        {
            name = "`isfunction` should return 'true' for a native function",
            func = function()
                expect(isfunction(isfunction)).to.beTrue()
            end
        },
        {
            name = "`isfunction` should return 'true' for a Lua-defined function",
            func = function()
                function hello()
                    print("Hello World!")
                end

                expect(isfunction(hello)).to.beTrue()
            end
        },
        {
            name = "`isfunction` should return 'false' for other types",
            func = function()
                expect(isfunction(nil)).to.beFalse()
                expect(isfunction(0)).to.beFalse()
                expect(isfunction("")).to.beFalse()
                expect(isfunction({})).to.beFalse()
            end
        },
    }
}
