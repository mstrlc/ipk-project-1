# IPK project 1 manual tests

## Other functionality

Test 1:
```
./ipkcpc -m aaa -p 2023 -h localhost
```
```
Error: Unknown mode 'aaa', must be either 'udp' or 'tcp'.
```

Test 2:
```
./ipkcpc -m tcp -p 1 -h localhost
```
```
Error: Port must be in range 1024-65535.
```

Test 3:
```
./ipkcpc -m tcp -p 65536 -h localhost
```
```
Error: Port must be in range 1024-65535.
```

Test 4:
```
./ipkcpc -m tcp -p 2023 -h 1.1.1.1.1
```
```
Error: No such host as 1.1.1.1.1
```

## UDP
Launch all with `./ipkcpc -m udp -p 2023 -h localhost`

Test 1:
```
(+ 15 25)
```
```
OK:40
```

Test 2:
```
(- 50 25)
```
```
OK:25
```

Test 3:
```
(* 7 3)
```
```
OK:21
```

Test 4:
```
(/ 100 5)
```
```
OK:20
```

Test 5:
```
(12 15 25)
```
```
ERR:not a valid expression
```

Test 6:
```
+ 2 3
```
```
ERR:not a valid expression
```

Test 7:
```
(* 99999 99999)
```
```
OK:9999800001
```


## TCP
Launch all with `./ipkcpc -m tcp -p 2023 -h localhost`

Test 1:
```
HELLO
SOLVE (+ 15 25)
BYE
```
```
HELLO
RESULT 40
BYE
```

Test 2:
```
HELLO
SOLVE (- 50 25)
BYE
```
```
HELLO
RESULT 25
BYE
```

Test 3:
```
HELLO
SOLVE (* 7 3)
BYE
```
```
HELLO
RESULT 21
BYE
```

Test 4:
```
HELLO
SOLVE (/ 100 5)
BYE
```
```
HELLO
RESULT 20
BYE
```

Test 5:
```
HELLO
BYE
```
```
HELLO
BYE
```

Test 6:
```
BYE
```
```
BYE
```

Test 7:
```
HELLO
HELLO
```
```
HELLO
BYE
```

Test 8:
```
HELLO
SOLVE (12 15 25)
```
```
HELLO
BYE
```

Test 9:
```
HELLO
SOLVE + 2 3
```
```
HELLO
BYE
```

Test 10:
```
HELLO
SOLVE (* 99999 99999)
BYE
```
```
HELLO
RESULT 9999800001
BYE
```