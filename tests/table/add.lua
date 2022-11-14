return {
    groupName = "Add",
    cases = {
        {
            name = "Add a sequentially numbered table to another sequentially numbered table",
            func = function()
                local target = {"One", "Two", "Three", "Four"}
                local source = {"Four", "Five", "Six"}
                expect(table.Add(target, source)).to.equal(target)
                expect(target[1]).to.equal("One")
                expect(target[2]).to.equal("Two")
                expect(target[3]).to.equal("Three")
                expect(target[4]).to.equal("Four")
                expect(target[5]).to.equal("Four")
                expect(target[6]).to.equal("Five")
                expect(target[7]).to.equal("Six")
            end
        },
    }
}
