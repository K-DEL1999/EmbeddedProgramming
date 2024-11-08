// TO FUTURE ME
// =============

// This was much simplier than the I2C protocol. It allowed me to get
// comfortable with the gptimer. The gptimer is a hardware timer while
// the high resolution esp timer is a software timer. I was able to use
// 8 us DELAY with the use of a hardware timer which was not possible 
// with the software timer. The periodic software timer capped at 50 us
// while the one time use timer capped at 20 us. Using this timer however
// resulted in some weird bugs at the beginning of transmission. The first
// packet was completely out of sync. So to accomodate I just send an
// empty packet to start. This project also allowed me to use the gdb 
// debugger for the first time and get comfortable with it. Its a very useful
// tool. I am very grateful we decided to learn how to use the gdb debugger.
// I was able to get to 125000 baudrate -- 8 us DELAY. Anything after that was
// too unstable. Even with the 125000 baudrate I was still getting a few 
// corrupted packets.


// THINGS TO IMPROVE
// ==================

// Figure out why the first packet is always corrupted. My guess is that its a system
// initialization issue, or the system taking longer to load the variables into the
// cache during runtime. 

// Try to correct the corrupted packets that come up once and a while

