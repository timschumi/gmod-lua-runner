return {
    groupName = "TypeID",
    cases = {
        {
            name = "`TypeID` should yield the correct type ID for builtin types",
            func = function()
                expect(TypeID("Hello")).to.equal(TYPE_STRING)
                expect(TypeID(function() end)).to.equal(TYPE_FUNCTION)
                expect(TypeID({})).to.equal(TYPE_TABLE)
                expect(TypeID(80)).to.equal(TYPE_NUMBER)
                expect(TypeID(false)).to.equal(TYPE_BOOL)
            end
        },
    }
}
