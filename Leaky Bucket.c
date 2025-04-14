#include <stdio.h>

int main() {
    int bsize, n, out, in, bucket = 0;

    printf("Enter the Bucket Size: ");
    scanf("%d", &bsize);

    printf("Enter the number of inputs (time units): ");
    scanf("%d", &n);

    printf("Enter the Outgoing rate: ");
    scanf("%d", &out);

    for (int i = 1; i <= n; i++) {
        printf("\nTime Unit %d:\n", i);
        printf("Enter the Incoming packet rate: ");
        scanf("%d", &in);

        if (in <= (bsize - bucket)) {
            bucket += in;
            printf("✅ Packets added to bucket. ");
        } else {
            int dropped = in - (bsize - bucket);
            printf("🚫 Packet Dropped: %d\n", dropped);
            bucket = bsize;
        }

        printf("📦 Bucket contains %d out of %d capacity\n", bucket, bsize);

        // Process outgoing packets
        if (bucket >= out) {
            bucket -= out;
            printf("📤 Outgoing %d packets.\n", out);
        } else {
            printf("📤 Outgoing %d packets.\n", bucket);
            bucket = 0;
        }

        printf("📦 After outgoing, bucket contains %d out of %d\n", bucket, bsize);
    }

    return 0;
}
