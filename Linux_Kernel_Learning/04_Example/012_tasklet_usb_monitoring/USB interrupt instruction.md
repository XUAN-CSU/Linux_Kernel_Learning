[ Physical USB Insertion ]
        ↓
[ Host Controller Hardware Interrupt ]
        ↓
[ Host Controller Driver (e.g., xhci-hcd, ehci-hcd, uhci-hcd) IRQ Handler ]
        ↓
[ USB Core Layer (drivers/usb/core/)]
        ↓
[ Enumeration Process (get device descriptor, assign address, etc.) ]
        ↓
[ USB Device Added Event Generated ]
        ↓
[ usb_register_notify() Notifier Chain Called ]
        ↓
[ Your Kernel Module’s notifier_call() Function Invoked ]
        ↓
[ (Optionally) Schedule a Tasklet or Workqueue for Deferred Processing ]


## 🔬 Step-by-Step Breakdown

### 1. **Hardware Level**

- You physically plug in a USB device.
    
- The **USB host controller (e.g., xHCI)** detects a _status change_ on one of its ports (e.g., “connect status changed”).
    
- It triggers a **hardware interrupt (IRQ)**.
    

---

### 2. **Interrupt Handler (Host Controller Driver)**

- The kernel’s **USB host controller driver** (for example `xhci-hcd.c`) handles this IRQ.
    
- This ISR (Interrupt Service Routine) runs in **hard interrupt context**, meaning:
    
    - It can’t sleep.
        
    - It runs extremely quickly.
        
- The ISR will read the port status and notice that a new device was connected.
    
- It schedules a **softirq or work** to handle the enumeration process.
    

---

### 3. **Enumeration & USB Core**

- The USB core (in `drivers/usb/core/hub.c`) takes over.
    
- It enumerates the new device:
    
    - Assigns an address.
        
    - Reads the device descriptor.
        
    - Creates `/sys/bus/usb/devices/...`.
        
- After this is done successfully, it triggers an **event**:
    
    `usb_notify_add_device(udev);`
    
- This function walks the **notifier chain** for USB events.
    

---

### 4. **usb_register_notify() Chain**

- Your module called `usb_register_notify(&usb_nb);`.
    
- So your `usb_nb.notifier_call()` is added to the global USB notifier chain.
    
- When the USB core emits an event (e.g., `USB_DEVICE_ADD`), your callback runs:
    
    `usb_event_notify(struct notifier_block *nb, unsigned long action, void *data)`
    

This callback runs in **process context** (not interrupt context), because enumeration already happened and notifications are sent from workqueue-like context inside the USB core.

---

### 5. **Your Callback (Kernel Module)**

- Your code receives:
    
    - `action` = `USB_DEVICE_ADD` or `USB_DEVICE_REMOVE`
        
    - `data` = pointer to `struct usb_device`
        
- At this point, the device is already fully known.
    
- Here you can:
    
    - Print messages (`pr_info`)
        
    - Collect info (VID/PID)
        
    - Or schedule a **tasklet or workqueue** for extra processing.
        

---

### 6. **(Optional) Tasklet/Workqueue**

If you want to simulate the “interrupt + bottom half” style in your module:

- You can treat the notifier callback as the **“top half”**.
    
- Then schedule a **tasklet** (runs in softirq context) or **workqueue** (runs in process context).
    

Example:

`DECLARE_TASKLET(usb_tasklet, usb_tasklet_func, 0);  static void usb_tasklet_func(unsigned long data) {     pr_info("USB_MON: tasklet running for deferred work\n"); }  static int usb_event_notify(struct notifier_block *nb, unsigned long action, void *data) {     if (action == USB_DEVICE_ADD)         tasklet_schedule(&usb_tasklet);     return NOTIFY_OK; }`

---

## 🧠 Summary Table

|Layer|Runs In|Who Triggers It|Context|Your Involvement|
|---|---|---|---|---|
|USB hardware|N/A|Device insertion|N/A|None|
|IRQ Handler (xhci/ehci)|Kernel (top-half ISR)|Hardware interrupt|Hard IRQ|None|
|USB core (hub.c)|Kernel thread/work|Enumeration finished|Process context|Indirect|
|usb_register_notify() callback|Kernel (core USB notifier)|USB core|Process context|✅ Your module runs here|
|Tasklet (optional)|Softirq|Scheduled by your callback|Softirq context|✅ Your module (deferred work)|

---

So in short:

> **You cannot directly attach to the raw USB IRQ**,  
> but you _can_ listen to USB device insertion/removal via `usb_register_notify()` —  
> and from that point, you may schedule your own **tasklet or workqueue**,  
> which is the kernel-approved way to do post-insertion work.