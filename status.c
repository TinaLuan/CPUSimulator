void status(int clock, list_t *memory, list_t *disk,
    queue_t *queue, int running_id);

void
status(int clock, list_t *memory, list_t *disk, queue_t *queue, int running_id) {
    if (clock == 116) {
        printf("clock: %d\n", clock);
        printf("\nrun: %d, %d\n", running_id, get_one(memory, running_id)->job_time);

        printf("\ndisk: %d\n", disk->num);
        printf("id\thole\n");
        process_t *each = disk->head;
        while (each) {
            printf("%d\t%d\n", each->pid, each->hole);
            each = each->next;
        }

        printf("\nmemory: %d\n", memory->num);
        printf("id\thole\n");
        each = memory->head;
        while (each) {
            printf("%d\t%d\n", each->pid, each->hole);
            each = each->next;
        }

        // queue
        printf("\nquque: %d\n", queue->num);
        printf("id\tjob\n");
        for (int i = 0; i < queue->num; i++) {
            printf("%d\t%d\n", queue->pid_arr[i], get_one(memory, queue->pid_arr[i])->job_time);
        }
    }
}
