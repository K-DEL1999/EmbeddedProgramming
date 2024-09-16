// TO FUTURE ME

// We ran this code with timer and saw that receive on average took about 
//  1 000 000 us because of its polling behavior -- waiting for response from
//  slave. If possible optimize.

// Also there is a bug where Master and Slave get stuck in a deadlock reset and return
//  to same deadlock. I am not sure if thats whats happening but I theorize that
//  Master is waiting to receive a LOW on the MISO line to being transmitting while the 
//  Slave is polling in the Slave receive function for rising edges. After the Slaves watchdog
//  activate -- clockWatchDog reaching 100 000 -- the slave resets inadvertently setting MISO
//  to LOW signaling the Master to transmit while no one is listening. As the master is sending 
//  Slave finished intialization and sees that the CS line is 0 which enables it to start polling 
//  for MOSI requests. Then it enters the loop for polling while the Master is again waiting 
//  for MISO to respond accordingly. (PLEASE SOLUTION TO THIS)

// STATS
//=======

Receive : 1 000 000 us
Transmit : 44 914 us 
