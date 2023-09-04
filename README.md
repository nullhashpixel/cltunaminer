# cltunaminer core
Experimental OpenCL based miner for TUNA

Please keep in mind this is an early experimental version of the miner, please report issues and the hash rates you get with your devices to the issue tracker on Github. 
Let's make it compatible with as many devices and operating systems as possible to improve decentralization!

**this miner core can only calculate hashes, for submitting transactions it needs this fork of the official repo:**
https://github.com/nullhashpixel/fortuna

tips: $gputunaminer 


# 1) Requirements
Only tested on Linux (Ubuntu 20) so far.

    apt install build-essential opencl-headers clinfo nvidia-opencl-dev

# 2) Download the repository

    git clone https://github.com/nullhashpixel/cltunaminer.git
    cd cltunaminer

# 3) Compiling from source

    make

# 4) Running the miner core

    ./cltuna

# 5) Start mining with GPU

*To be able to mine, you also need the custom fork of the TypeScript transaction building code from:*
https://github.com/nullhashpixel/fortuna

- Install and compile the miner core, see above steps 1-4

- Install the fortuna code and setup your wallet as described in the fortuna repository
  
- Start the mining core(s): ```./cltuna```

![Screenshot miner core started](./core_started.png)

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

- check the window of your GPU miner core, it should show you hash rates, estimates for how often solutions are found and the current difficulty
![Screenshot miner core running](./core_running.png)

# Hash rates for GPUs

|model   | OS  | hash rate  |
|---|---|---|
|RTX A4000   | Linux  |1.0 GH/s   |
|RTX 2080S   | Linux  |1.1GH/s   |
|RTX 6000   | Linux  |3.5 GH/s   |
|L40   |   Linux  |3.7 GH/s  |
|RTX 3090   | Linux  |1.8 GH/s   |
|RTX 4090   | Linux  |3.8 GH/s   |


# Advanced stuff

## Running the miner core and making it available to outside machines in the network on port 12345

    ./cltuna 0.0.0.0 12345

## Benchmark
Running the benchmark will not wait for connection from the mining script, just computes the hash rate.

    ./cltuna benchmark

## Automatic restart on crashes
If you experience occasional crashes of the miner, consider running it in an infinite loop with a delay.

    while true; do ./a.out; sleep 1; done


# Known bugs/limitations

    Multi GPU support
    AMD cards untested
    Windows support

# Disclaimer

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
