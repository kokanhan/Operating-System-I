# CS344_adventure
CS344 Operating Systems: Program 2 - adventure

Some codes for directory part and mutex part are from lecture notes and piazza questions.  
The jianghan.buildroom.c was created when I first time took CS344.
The indentation of all blocks is completed and looks perfect on os1 

```html
os1 ~/cs344/adventure 1133$ gcc -o jianghan.buildrooms jianghan.buildrooms.c
os1 ~/cs344/adventure 1134$ jianghan.buildrooms
os1 ~/cs344/adventure 1135$ gcc -o jianghan.adventure jianghan.adventure.c -lpth                                                                                                                               read
os1 ~/cs344/adventure 1136$ jianghan.adventure
CURRENT LOCATION: Cafe
POSSIBLE CONNECTIONS: Kitchen, Laundry, Visitor, Security
WHERE TO? >^C
os1 ~/cs344/adventure 1137$ valgrind --tool=memcheck --leak-check=full --show-le                                                                                                                               ak-kinds=all ./jianghan.adventure
==25710== Memcheck, a memory error detector
==25710== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==25710== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==25710== Command: ./jianghan.adventure
==25710==
CURRENT LOCATION: Cafe
POSSIBLE CONNECTIONS: Kitchen, Laundry, Visitor, Security
WHERE TO? >Security

YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!
YOU TOOK 1 STEPS. YOUR PATH TO VICTORY WAS:
Security
==25710==
==25710== HEAP SUMMARY:
==25710==     in use at exit: 0 bytes in 0 blocks
==25710==   total heap usage: 17 allocs, 17 frees, 70,390 bytes allocated
==25710==
==25710== All heap blocks were freed -- no leaks are possible
==25710==
==25710== For counts of detected and suppressed errors, rerun with: -v
==25710== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
os1 ~/cs344/adventure 1138$ valgrind --tool=memcheck --leak-check=full --show-le                                                                                                                               ak-kinds=all ./jianghan.adventure
==25800== Memcheck, a memory error detector
==25800== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==25800== Using Valgrind-3.13.0 and LibVEX; rerun with -h for copyright info
==25800== Command: ./jianghan.adventure
==25800==
CURRENT LOCATION: Cafe
POSSIBLE CONNECTIONS: Kitchen, Laundry, Visitor, Security
WHERE TO? >Laundry

CURRENT LOCATION: Laundry
POSSIBLE CONNECTIONS: Cafe, Bed, Kitchen, Security
WHERE TO? >time

  8:48pm, Saturday, November 02, 2019

WHERE TO? >Bed

CURRENT LOCATION: Bed
POSSIBLE CONNECTIONS: Visitor, Laundry, Meeting, Kitchen
WHERE TO? >hfdoighr

HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.

CURRENT LOCATION: Bed
POSSIBLE CONNECTIONS: Visitor, Laundry, Meeting, Kitchen
WHERE TO? >Meeting

CURRENT LOCATION: Meeting
POSSIBLE CONNECTIONS: Bed, Kitchen, Visitor
WHERE TO? >Kitchen

CURRENT LOCATION: Kitchen
POSSIBLE CONNECTIONS: Cafe, Visitor, Laundry, Meeting, Bed
WHERE TO? >Cafe

CURRENT LOCATION: Cafe
POSSIBLE CONNECTIONS: Kitchen, Laundry, Visitor, Security
WHERE TO? >Security

YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!
YOU TOOK 6 STEPS. YOUR PATH TO VICTORY WAS:
Laundry
Bed
Meeting
Kitchen
Cafe
Security
==25800==
==25800== HEAP SUMMARY:
==25800==     in use at exit: 0 bytes in 0 blocks
==25800==   total heap usage: 28 allocs, 28 frees, 73,845 bytes allocated
==25800==
==25800== All heap blocks were freed -- no leaks are possible
==25800==
==25800== For counts of detected and suppressed errors, rerun with: -v
==25800== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
