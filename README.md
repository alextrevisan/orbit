orbit - a simple Arduino script.
=====
How about send commands via Serial or loading a script from SD card to execute on Arduino?
This is a simple script command to execute operations on Arduino via script.


Try to connect via serial and send commands like this:
```
pinMode 13 OUTPUT
digitalWrite 13 HIGH
```
or using a motor shield:
```
pinMode 3 OUTPUT
pinMode 5 OUTPUT
pinMode 6 OUTPUT
analogWrite 5 0
analogWrite 6 20
digitalWrite 3 HIGH
```
How to create a custom function:

1) create a function always return void e no parameters. if you want return parameters use orbit_pushstack(char_value) or orbit_pushinteger(int_value)
```
void examplefunction()
{
	//if you call: examplefunction 13 HIGH
	int mode = orbit_popstack(); //get the "HIGH" variable
	int pin = orbit_popstack(); //get the "13" variable
	digitalWrite(pin,mode);
}
```
2) register the function before you call orbit_run()
```
orbit_registerfunction("examplefunction",examplefunction);
```
3) call from script:
```
examplefunction 13 HIGH
```
Variables:
```
var value = analogRead 3
analogWrite 5 value
```
- [x] execute functions
- [x] store variables
- [x] read variables
- [ ] perform operations
- [ ] if keyword
- [ ] for keyword
- [ ] while keyword
- [ ] more and more...

