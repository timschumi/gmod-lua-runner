return {
    groupName = "type",
    cases = {
        {
            name = "`type` should return 'no value' if no argument is given",
            func = function()
                expect(type()).to.equal("no value")
            end
        },
        {
            name = "`type` should the correct type name for builtin types",
            func = function()
                expect(type(0)).to.equal("number")
                expect(type("")).to.equal("string")
                expect(type({})).to.equal("table")
            end
        },
    }
}