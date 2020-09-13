# Variant++
A super simple Header-only wrapper for ProtonSDK Variant.

## Installation
Just simply include the header, and you're good!  
Do take note that this requires **C++17** and above.

## Usage
```cpp
#include "./Variant.hpp"
#include <cstdio>

Variant variant; // initiate variant class
variant << "OnConsoleMessage" << "Hello World!"; // insert those two arguments to the variant

unsigned char* data; // declare an uninitialized char pointer
variant >> &data; // output the data
// operator = works as well for outputing the data

auto size = variant.size(); // return the size of the bytes
for (int i = 0; i < size; ++i) {
	printf("%02X ", data[i]); // print each int as hex
}

printf("\n");
```
You can also use the `+` operator  
```cpp
#include "./Variant.hpp"

Variant variant;
std::vector<float> vec { 0.0f, 10.0f };

variant + "OnSetPos" + vec; // float vectors work
```
Another example  
```cpp
#include "./Variant.hpp"

Variant variant;
variant + "OnSuperMain..." + 100000 + "..."; // yes and ints work, both unsigned and signed
```  

**Fetching Arguments**
```cpp
#include "./Variant.hpp"
#include <iostream>

Variant variant; // initiate variant class
variant << "OnConsoleMessage" << "Hello World!"; // insert those two arguments to the variant

std::cout << variant.at<std::string>(0) << std::endl;
```  
If you don't know the type, there's an alternative.  
```cpp
#include "./Variant.hpp"
#include <iostream>

Variant variant; // initiate variant class
variant << "OnConsoleMessage" << "Hello World!"; // insert those two arguments to the variant

auto type = variant.type(0); // check the type of the arg at index 0
if (type == VariantArgTypes::STRING) {
	std::cout << "That arg was a string!" << std::endl;
} else { /* handle other types...*/ }
```