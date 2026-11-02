// Edge Silicon eBPF Ingress Interlock Filter
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Target Subsystem: Linux SO_ATTACH_BPF (BPF_PROG_TYPE_SOCKET_FILTER)
// Return Semantics:
//   - 0 (BPF_DROP): Truncates and discards entire frame from socket receive buffer.
//   - skb->len (BPF_PASS): Delivers full intact packet payload to userspace socket queue.
// Note: This operates at the socket ingress layer, not TC or XDP.

#include <linux/bpf.h>

#ifndef SEC
#define SEC(NAME) __attribute__((section(NAME), used))
#endif

#define MAX_PAYLOAD_SIZE 4096
#define BPF_DROP 0

SEC("socket")
int filter_uedo_packets(struct __sk_buff *skb) {
    // 1. Guard against empty/zero-length frames
    if (skb->len == 0) {
        return BPF_DROP;
    }

    // 2. Enforce strict 4096-byte hardware static boundary (S_max <= 4096B)
    if (skb->len > MAX_PAYLOAD_SIZE) {
        return BPF_DROP; // Discard oversized frame at kernel ingress
    }

    // 3. Retain complete packet buffer and pass to PREEMPT_RT application space
    return skb->len;
}

char _license[] SEC("license") = "GPL";
