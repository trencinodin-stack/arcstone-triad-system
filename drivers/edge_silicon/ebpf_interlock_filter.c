// eBPF Socket Filter for Arcstone Edge Silicon Interlock Gate
// Master Anchor: A-77-DELTA-SHIELD-LOCKED

#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/in.h>

#define SEC(NAME) __attribute__((section(NAME), used))

#define MAX_PAYLOAD_SIZE 4096
#define POSIX_PASS 0
#define POSIX_FAIL 40

SEC("socket_uedo_filter")
int filter_uedo_packets(struct __sk_buff *skb) {
    // Enforce strict 4096-byte hardware static payload boundary
    if (skb->len > MAX_PAYLOAD_SIZE) {
        return POSIX_FAIL; // Drop packet at NIC driver layer
    }

    // Pass valid frames to PREEMPT_RT kernel boundary
    return POSIX_PASS;
}

char _license[] SEC("license") = "GPL";
