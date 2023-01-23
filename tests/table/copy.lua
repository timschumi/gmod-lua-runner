return {
    groupName = "Copy",
    cases = {
        {
            name = "Copy an empty table",
            func = function()
                local old_table = {}
                local new_table = table.Copy(old_table)
                expect(new_table).to.beA("table")
                expect(table.Count(new_table)).to.equal(0)
            end
        },
        {
             name = "Copy a numbered table with primitive values",
             func = function()
                 local old_table = { 0, 1, 2, "a", "b", 3, "c" }
                 local new_table = table.Copy(old_table)
                 expect(new_table).to.beA("table")
                 expect(table.Count(new_table)).to.equal(7)
                 expect(new_table[1]).to.equal(0)
                 expect(new_table[2]).to.equal(1)
                 expect(new_table[3]).to.equal(2)
                 expect(new_table[4]).to.equal("a")
                 expect(new_table[5]).to.equal("b")
                 expect(new_table[6]).to.equal(3)
                 expect(new_table[7]).to.equal("c")

                 new_table[3] = "e"
                 new_table[4] = 5

                 expect(old_table[3]).to.equal(2)
                 expect(old_table[4]).to.equal("a")
                 expect(new_table[3]).to.equal("e")
                 expect(new_table[4]).to.equal(5)
             end
        },
        {
             name = "Copy a mixed-key table with primitive values",
             func = function()
                 local old_table = {
                     "a",
                     "b",
                     [123] = "foo",
                     "c",
                     name = "bar",
                     "d",
                     "e",
                 }
                 local new_table = table.Copy(old_table)
                 expect(new_table).to.beA("table")
                 expect(table.Count(new_table)).to.equal(7)

                 expect(new_table[1]).to.equal("a")
                 expect(new_table[2]).to.equal("b")
                 expect(new_table[3]).to.equal("c")
                 expect(new_table[4]).to.equal("d")
                 expect(new_table[5]).to.equal("e")
                 expect(new_table[123]).to.equal("foo")
                 expect(new_table["name"]).to.equal("bar")
             end
        },
        {
             name = "Copy a nested table",
             func = function()
                 local old_table = {
                     "a",
                     "b",
                     {
                         "c",
                         "d",
                         "e",
                     },
                     "f",
                     "g",
                 }
                 local new_table = table.Copy(old_table)
                 expect(new_table).to.beA("table")
                 expect(table.Count(new_table)).to.equal(5)

                 expect(new_table[1]).to.equal("a")
                 expect(new_table[2]).to.equal("b")
                 expect(new_table[3][1]).to.equal("c")
                 expect(new_table[3][2]).to.equal("d")
                 expect(new_table[3][3]).to.equal("e")
                 expect(new_table[4]).to.equal("f")
                 expect(new_table[5]).to.equal("g")

                 new_table[3][2] = "D"

                 expect(old_table[3][2]).to.equal("d")
             end
        },
    }
}
