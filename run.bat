for /l %%i in (1,1,100) do (
  .\sqlite_use_in >> .\sqlite_use_in.txt
)

for /l %%i in (1,1,100) do (
  .\sqlite_not_use_in >> .\sqlite_not_use_in.txt
)
