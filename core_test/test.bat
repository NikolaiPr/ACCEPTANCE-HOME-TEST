SET app=l:\PROEKSPERT-HOME-TEST\bin\Release_x64\console.exe
SET dir=L:\PROEKSPERT-HOME-TEST\core_test\test_files\

call %app% %dir%empty.txt 10 %%
call %app% %dir%default.txt 10 %%
call %app% %dir%3res.txt 10
 %%
call %app% %dir%nores.txt 10 %%
call %app% %dir%oneletter.txt 10 %%
call %app% %dir%sixletters.txt 10 %%
call %app% %dir%m_nores.txt 10 %%
call %app% %dir%m_sixletters.txt 10 %%
call %app% %dir%m_threeletters.txt 10 %%
call %app% %dir%twoletters.txt 10 %%
call %app% %dir%symbols.txt 10 %%
call %app% %dir%large.txt 10 %%

