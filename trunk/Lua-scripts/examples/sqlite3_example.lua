-- See Copyright Notice in license.html

et.G_Printf = function(...)
		et.G_Print(string.format(unpack(arg)))
end

-- load driver
require "luasql.sqlite3"
-- create environment object
env = assert (luasql.sqlite3())
-- connect to data source
con = assert (env:connect("luasql-test"))
-- reset our table
res = con:execute"DROP TABLE people"
res = assert (con:execute[[
  CREATE TABLE people(
    name  varchar(50),
    email varchar(50)
  )
]])
-- add a few elements
list = {
  { name="Jose das Couves", email="jose@couves.com", },
  { name="Manoel Joaquim", email="manoel.joaquim@cafundo.com", },
  { name="Maria das Dores", email="maria@dores.com", },
}
for i, p in pairs (list) do
  res = assert (con:execute(string.format([[
    INSERT INTO people
    VALUES ('%s', '%s')]], p.name, p.email)
  ))
end
-- retrieve a cursor
cur = assert (con:execute"SELECT name, email from people")
-- print all rows
row = cur:fetch ({}, "a")	-- the rows will be indexed by field names
while row do
  et.G_Printf("Name: %s, E-mail: %s\n", row.name, row.email)
  row = cur:fetch (row, "a")	-- reusing the table of results
end
-- close everything
cur:close()
con:close()
env:close()
