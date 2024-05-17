# Expresso

Old school 3D engine.

## Dev Env

```
sudo apt install build-essential
sudo apt install libsdl2-dev
```

---

## Notes


3D points:
```
y
|
|   z   . (2, 2, 1)
|  /   
| /
|/________ x
```

**Rotation**
REM: SOH CAH TOA ;)
```
Rotation is the following
- We have:
    x = r cos(a) and y = r sin (a)
- We want
    x' = r cos(a+b) and y' = r sin(a+b)

- Then with the angle addition formula for cosine
x' = r cos(a + b)
x' = r (cos(a) cos(b) - sin(a) sin(b))
x' = r cos(a) cos(b) - r sin(a) sin(b)
x' = x cos(b) - y sin(b)

So then the matrix will be:
[cos(a) -sin(a)] [x]
[sin(a)  cos(a)] [y]

And extending to the next dimention:

[cos(a) -sin(a)] [x]
[sin(a)  cos(a)] [y]
[              ] [z]
```
