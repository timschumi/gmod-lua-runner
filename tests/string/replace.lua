return {
    groupName = "Replace",
    cases = {
        {
            name = "Empty input",
            func = function()
                expect(string.Replace("", "A", "a")).to.equal("")
            end
        },
        {
            name = "Empty replacement",
            func = function()
                expect(string.Replace("ABC", "B", "")).to.equal("AC")
            end
        },
        {
            name = "Replacements at start",
            func = function()
                expect(string.Replace("ABC", "A", "")).to.equal("BC")
                expect(string.Replace("ABC", "A", "AB")).to.equal("ABBC")
            end
        },
        {
            name = "Replacements at end",
            func = function()
                expect(string.Replace("ABC", "C", "")).to.equal("AB")
                expect(string.Replace("ABC", "C", "BA")).to.equal("ABBA")
            end
        },
        {
            name = "Multiple consecutive replacements",
            func = function()
                expect(string.Replace("AAAAAA", "A", "a")).to.equal("aaaaaa")
            end
        },
        {
            name = "Multiple disjunct replacements",
            func = function()
                expect(string.Replace("ABABA", "A", "a")).to.equal("aBaBa")
            end
        },
    }
}
