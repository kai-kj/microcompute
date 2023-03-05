# `microcompute`

[**Github**](https://github.com/kal39/microcompute)

Usage notes:
- For `gcc`, use `-lmicrocompute -lgbm -lEGL -lGL -lGLEW`
- Because this library uses `OpenGL`, all `microcompute` operations must be
performed in the same thread
- For examples, see
[here](https://github.com/kal39/microcompute/blob/master/example)

## Documentation
### Structs
- ```c
  typedef enum mc_Bool {
      MC_FALSE = 0,
      MC_TRUE = 1,
  } mc_Bool;
  ```
  Bool type.

<br/>

- ```c
  typedef struct mc_Result {
      mc_Bool ok;
      uint32_t line;
      const char* file;
      const char* func;
      const char* message;
  } mc_Result;
  ```
  Result type. Returned by all microcompute functions.
  - `ok`: `MC_TRUE` if no errors, `MC_FALSE` otherwise
  - `line`: The line number where the result was generated
  - `file`: The name of the file where the result was generated
  - `func`: The name of the function where the result was generated
  - `message`: A message attached to the result

<br/>

- ```c
  typedef struct mc_Program mc_Program;
  typedef struct mc_Buffer mc_Buffer;
  ```
  Private microcompute types.

<br/>

- ```c
  // float vectors
  typedef struct mc_vec2 {
      float x, y;
  } mc_vec2;
  
  typedef struct mc_vec3 {
      float x, y, z;
  } mc_vec3;
  
  typedef struct mc_vec4 {
      float x, y, z, w;
  } mc_vec4;
  
  // int vectors
  typedef struct mc_ivec2 {
      int32_t x, y;
  } mc_ivec2;
  
  typedef struct mc_ivec3 {
      int32_t x, y, z;
  } mc_ivec3;
  
  typedef struct mc_ivec4 {
      int32_t x, y, z, w;
  } mc_ivec4;
  
  // uint vectors
  typedef struct mc_uvec2 {
      uint32_t x, y;
  } mc_uvec2;
  
  typedef struct mc_uvec3 {
      uint32_t x, y, z;
  } mc_uvec3;
  
  typedef struct mc_uvec4 {
      uint32_t x, y, z, w;
  } mc_uvec4;
  ```
  Vector types, compatible with glsl vectors.

<br/>

- ```c
  typedef struct mc_mat22 {
      float values[4];
      mc_Bool transpose;
  } mc_mat22;
  
  typedef struct mc_mat33 {
      float values[9];
      mc_Bool transpose;
  } mc_mat33;
  
  typedef struct mc_mat44 {
      float values[16];
      mc_Bool transpose;
  } mc_mat44;
  
  typedef struct mc_mat23 {
      float values[6];
      mc_Bool transpose;
  } mc_mat23;
  
  typedef struct mc_mat32 {
      float values[6];
      mc_Bool transpose;
  } mc_mat32;
  
  typedef struct mc_mat24 {
      float values[8];
      mc_Bool transpose;
  } mc_mat24;
  
  typedef struct mc_mat42 {
      float values[8];
      mc_Bool transpose;
  } mc_mat42;
  
  typedef struct mc_mat34 {
      float values[12];
      mc_Bool transpose;
  } mc_mat34;
  
  typedef struct mc_mat43 {
      float values[12];
      mc_Bool transpose;
  } mc_mat43;
  ```
  Matrix types, compatible with glsl matrices.
  
  If `transpose` is set to true, the matrix values are in row major order,
  else, the values are in column major order. It is passed as the `transpose`
  argument to the `glUniformMatrix()` functions.

<br/>

### Enums
- ```c
  typedef enum mc_DebugLevel {
      mc_DebugLevel_INFO,
      mc_DebugLevel_LOW,
      mc_DebugLevel_MEDIUM,
      mc_DebugLevel_HIGH,
  } mc_DebugLevel;
  ```
  Debug message severity level.
  
  Passed as an argument to the debug callback function.

<br/>

### Core functionality
- ```c
  mc_Result mc_start(char* renderDevice);
  ```
  Initialize the microcompute library. If any errors occur, it will call the
  debug callback function, so `mc_set_debug_callback()` should be called before
  calling `mc_start()`.
  
  If the machine the library is being used on has multiple graphics cards, the
  `renderDevice` parameter will determine which card is used.
  
  - `renderDevice`: The rendering device to be used by the library, should be
  `/dev/dri/renderD...`
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  mc_Result mc_stop();
  ```
  Stops the microcompute library.
  
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

### Program (compute shader) management
- ```c
  mc_Program* mc_program_from_string(
      const char* code,
      uint32_t maxErrorLength,
      char* error
  );
  ```
  Create a program (compute shader) from a string.
  
  - `program`: The program struct to initialize
  - `code`: A string containing the shader code
  - `maxErrorLength`: Maximum length for shader errors
  - `error`: A string with longer than `maxErrorLength` that will contain any
  shader errors
  - returns: A non-`NULL` pointer on success, `NULL` otherwise

<br/>

- ```c
  mc_Program* mc_program_from_file(
      const char* path,
      uint32_t maxErrorLength,
      char* error
  );
  ```
  Create a program (compute shader) from a file.
  
  - `program`: The program struct to initialize
  - `code`: A string containing the path to the file containing the shader code
  - `maxErrorLength`: Maximum length for shader errors
  - `error`: A string with longer than `maxErrorLength` that will contain any
  shader errors
  - returns: A non-`NULL` pointer on success, `NULL` otherwise

<br/>

- ```c
  mc_Result mc_program_destroy(mc_Program* program);
  ```
  Destroy a program.
  
  - `program`: The program to destroy
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  mc_Result mc_program_dispatch(
      mc_Program* program,
      mc_ivec3 size,
      uint32_t bufferCount,
      mc_Buffer** buffers
  );
  ```
  Dispatch (run) a program.
  
  - `program`: The program to run
  - `size`: The number of workgroups to be run in each dimension
  - `bufferCount`: The number of buffers to pass to the program
  - `buffers`: The buffers to pass to the program, the the buffers will be
  bound according to their index in this array
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  // for float values
  mc_Result mc_program_set_float(mc_Program* program, char* name, float value);
  mc_Result mc_program_set_vec2(mc_Program* program, char* name, mc_vec2 value);
  mc_Result mc_program_set_vec3(mc_Program* program, char* name, mc_vec3 value);
  mc_Result mc_program_set_vec4(mc_Program* program, char* name, mc_vec4 value);
  
  // for int values
  mc_Result mc_program_set_int(mc_Program* program, char* name, int32_t value);
  mc_Result mc_program_set_ivec2(mc_Program* program, char* name, mc_ivec2 value);
  mc_Result mc_program_set_ivec3(mc_Program* program, char* name, mc_ivec3 value);
  mc_Result mc_program_set_ivec4(mc_Program* program, char* name, mc_ivec4 value);
  
  // for uint values
  mc_Result mc_program_set_uint(mc_Program* program, char* name, uint32_t value);
  mc_Result mc_program_set_uvec2(mc_Program* program, char* name, mc_uvec2 value);
  mc_Result mc_program_set_uvec3(mc_Program* program, char* name, mc_uvec3 value);
  mc_Result mc_program_set_uvec4(mc_Program* program, char* name, mc_uvec4 value);
  
  // for matrix values
  mc_Result mc_program_set_mat22(mc_Program* program, char* name, mc_mat22 value);
  mc_Result mc_program_set_mat33(mc_Program* program, char* name, mc_mat33 value);
  mc_Result mc_program_set_mat44(mc_Program* program, char* name, mc_mat44 value);
  mc_Result mc_program_set_mat23(mc_Program* program, char* name, mc_mat23 value);
  mc_Result mc_program_set_mat32(mc_Program* program, char* name, mc_mat32 value);
  mc_Result mc_program_set_mat24(mc_Program* program, char* name, mc_mat24 value);
  mc_Result mc_program_set_mat42(mc_Program* program, char* name, mc_mat42 value);
  mc_Result mc_program_set_mat34(mc_Program* program, char* name, mc_mat34 value);
  mc_Result mc_program_set_mat43(mc_Program* program, char* name, mc_mat43 value);
  ```
  Set the value of uniform value.
  
  - `program`: The program in which to set the uniform value
  - `name`: The name of the uniform to set
  - `value`: The value of the uniform
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

### Buffer management
- ```c
  mc_Buffer* mc_buffer_create(uint64_t size);
  ```
  Create a buffer (SSBO).
  
  - `buffer`: The buffer struct to initialize
  - `size`: The size of the buffer
  - returns: A non-`NULL` pointer on success, `NULL` otherwise

<br/>

- ```c
  mc_Result mc_buffer_destroy(mc_Buffer* buffer);
  ```
  Destroy a buffer.
  
  - `buffer`: The buffer to destroy
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  mc_Result mc_buffer_resize(mc_Buffer* buffer, uint64_t size);
  ```
  Resize a buffer.
  
  - `buffer`: The buffer to resize
  - `binding`: The (new) size of the buffer
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  mc_Result mc_buffer_get_size(mc_Buffer* buffer, uint64_t* size);
  ```
  Get the current size of a buffer.
  
  - `buffer`: The buffer to get the size of
  - returns: The current size of the buffer
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  mc_Result mc_buffer_write(
      mc_Buffer* buffer,
      uint64_t offset,
      uint64_t size,
      void* data
  );
  ```
  Write data to a buffer. If `off` + `size` is larger than the size of the
  buffer, the function call will fail.
  
  - `buffer`: The buffer to write to
  - `off`: The offset at which to start writing data to, measured in bytes
  - `size`: The size (length) of the data to be written, measured in bytes
  - `data`: The data to write
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

- ```c
  mc_Result mc_buffer_read(
      mc_Buffer* buffer,
      uint64_t offset,
      uint64_t size,
      void* data
  );
  ```
  Read data from a buffer. If `off` + `size` is larger than the size of the
  buffer, the function call will fail.
  
  - `buffer`: The buffer to read from
  - `off`: The offset at which to start reading data from, measured in bytes
  - `size`: The size (length) of the data to be read, measured in bytes
  - `data`: A buffer to write the data into. Must be pre-allocated
  - returns: `mc_Result` with `ok = MC_TRUE` on success, `ok = MC_FALSE`
  otherwise

<br/>

### Misc functions
- ```c
  void mc_result_pretty_print(mc_Result result);
  ```
  Print the contents of a `mc_Result`.
  
  - `result`: The result to print

<br/>

## License

```
MIT License
Copyright (c) 2023 Kai Kitagawa-Jones

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
