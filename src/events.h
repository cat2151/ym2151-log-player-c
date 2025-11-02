#include "types.h"

// Initialize register event list
RegisterEventList *create_event_list()
{
    RegisterEventList *list = (RegisterEventList *)malloc(sizeof(RegisterEventList));
    if (!list)
    {
        fprintf(stderr, "❌ Failed to allocate memory for event list\n");
        exit(1);
    }
    list->capacity = 256;
    list->count = 0;
    list->events = (RegisterEvent *)malloc(sizeof(RegisterEvent) * list->capacity);
    if (!list->events)
    {
        fprintf(stderr, "❌ Failed to allocate memory for events array\n");
        free(list);
        exit(1);
    }
    return list;
}

// Add event to list with is_data_write flag
void add_event_with_flag(RegisterEventList *list, uint32_t sample_time, uint8_t address, uint8_t data, uint8_t is_data_write)
{
    if (list->count >= list->capacity)
    {
        list->capacity *= 2;
        RegisterEvent *new_events = (RegisterEvent *)realloc(list->events, sizeof(RegisterEvent) * list->capacity);
        if (!new_events)
        {
            fprintf(stderr, "❌ Failed to reallocate memory for events\n");
            exit(1);
        }
        list->events = new_events;
    }
    list->events[list->count].sample_time = sample_time;
    list->events[list->count].address = address;
    list->events[list->count].data = data;
    list->events[list->count].is_data_write = is_data_write;
    list->count++;
}

// Free event list
void free_event_list(RegisterEventList *list)
{
    free(list->events);
    free(list);
}

// Calculate samples for a duration at internal sample rate
uint32_t duration_to_samples(double duration_seconds)
{
    return (uint32_t)(duration_seconds * INTERNAL_SAMPLE_RATE);
}

// Calculate total playback duration from events
double calculate_playback_duration(RegisterEventList *events)
{
    if (events->count == 0)
        return 1.0;

    // Find last event time
    uint32_t last_event_time = 0;
    for (size_t i = 0; i < events->count; i++)
    {
        if (events->events[i].sample_time > last_event_time)
        {
            last_event_time = events->events[i].sample_time;
        }
    }

    // Add 1 second after last event
    uint32_t total_samples = last_event_time + INTERNAL_SAMPLE_RATE;
    double duration = (double)total_samples / INTERNAL_SAMPLE_RATE;

    printf("Playback duration calculation:\n");
    printf("  Last event at: %u samples (%.3f seconds)\n",
           last_event_time, (double)last_event_time / INTERNAL_SAMPLE_RATE);
    printf("  Total duration: %.3f seconds (%u samples)\n\n", duration, total_samples);

    return duration;
}
