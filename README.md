# cltunaminer core
Experimental OpenCL based miner for TUNA

Please keep in mind this is an early experimental version of the miner, please report issues and the hash rates you get with your devices to the issue tracker on Github. 

*To be able to mine, you also need the custom fork of the TypeScript transaction building code.*

- Start the mining core(s): ```./cltuna```
- Add the hostname(s) of your miner cores to the **main.ts** file of the mining code in the *cltunaminer* directory of the TypeScript code

Simple example for running with 1 local GPU:
````
const miners = [
        {port: 2023}
]
````
Running the code on two different machines in your network:
````
const miners = [
        {hostname: "192.168.0.100", port: 2023},
        {hostname: "192.168.0.101", port: 2023}
]
````

- (in a different terminal window): Start the TypeScript mining script with deno: ```deno task cltunaminer mine```

  
# Requirements
Only tested on Linux (Ubuntu 20) so far.

    apt install build-essential opencl-headers clinfo nvidia-opencl-dev

# Compiling from source

    make

# Running the miner core

    ./cltuna

# Running the miner core and making it available to outside machines in the network on port 12345

    ./cltuna 0.0.0.0 12345

# Benchmark
Running the benchmark will not wait for connection from the mining script, just computes the hash rate.

    ./cltuna benchmark

# Automatic restart on crashes
If you experience occasional crashes of the miner, consider running it in an infinite loop with a delay.

    while true; do ./a.out; sleep 1; done


# Changelog

````
v1.1: 23/09/05 Windows compatibility
v1.0: 23/09/04 initial release
````

# Known bugs/limitations

    Multi GPU support
    AMD cards untested

# Disclaimer

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
