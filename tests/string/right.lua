return {
    groupName = "Right",
    cases = {
        {
            name = "Normal case",
            func = function()
                expect(string.Right("abc", 1)).to.equal("c")
                expect(string.Right("abc", 2)).to.equal("bc")
                expect(string.Right("abc", 3)).to.equal("abc")
            end
        },
        {
            name = "Number larger than string length",
            func = function()
                expect(string.Right("abc", 4)).to.equal("abc")
                expect(string.Right("abc", 5)).to.equal("abc")
            end
        },
        {
            name = "Number smaller than string length",
            func = function()
                expect(string.Right("abc", 0)).to.equal("abc")
                expect(string.Right("abc", -1)).to.equal("abc")
                expect(string.Right("abc", -2)).to.equal("bc")
                expect(string.Right("abc", -3)).to.equal("c")
                expect(string.Right("abc", -4)).to.equal("")
                expect(string.Right("abc", -5)).to.equal("")
            end
        },
        {
            name = "String length is zero",
            func = function()
                expect(string.Right("", 1)).to.equal("")
                expect(string.Right("", 0)).to.equal("")
                expect(string.Right("", -1)).to.equal("")
            end
        },
    }
}
