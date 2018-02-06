/*
 *
 *    Copyright (c) 2015 Nest Labs, Inc.
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file implements a unit test for the NLER one-shot timer
 *      interfaces.
 */

#include <stdio.h>
#include <string.h>
#include "nlertask.h"
#include "nlerinit.h"
#ifdef nlLOG_PRIORITY
#undef nlLOG_PRIORITY
#endif
#define nlLOG_PRIORITY 4
#include "nlerlog.h"
#include "nlereventqueue.h"
#include "nlertimer.h"
#if NLER_FEATURE_SIMULATEABLE_TIME
#include "nlertimer_sim.h"
#endif

nl_task_t taskA;
uint8_t stackA[NLER_TASK_STACK_BASE + 128];

struct taskAData
{
    nl_eventqueue_t mTimer;
    nl_eventqueue_t mQueue;
};

#if NLER_FEATURE_SIMULATEABLE_TIME

nl_task_t taskB;
uint8_t stackB[NLER_TASK_STACK_BASE + 128];

void taskEntryB(void *aParams)
{
    (void)aParams;
    const nl_time_ms_t advance_time_ms = 4000;
    const nl_time_ms_t sleep_time_ms = 4000;
    nl_time_native_t now;
    while (1)
    {
        if (nl_is_time_paused()) {
            now = nl_get_time_native();
            NL_LOG_CRIT(lrTEST, "[P, B] %u advancing %u ms\n", now, advance_time_ms);
            NL_LOG_CRIT(lrTEST, "∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨∨\n");
            nl_advance_time_ms(advance_time_ms);
            NL_LOG_CRIT(lrTEST, "∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧∧\n");

            now = nl_get_time_native();
            NL_LOG_CRIT(lrTEST, "[%c, B, %u] about to unpause\n", nl_is_time_paused() ? 'P' : 'U', now);
            nl_unpause_time();
            now = nl_get_time_native();
            NL_LOG_CRIT(lrTEST, "[%c, B, %u] about to sleep for %ums\n", nl_is_time_paused() ? 'P' : 'U', now, sleep_time_ms);
            nl_task_sleep_ms(sleep_time_ms);
        } else {
            now = nl_get_time_native();
            NL_LOG_CRIT(lrTEST, "[%c, B, %u] about to pause\n", nl_is_time_paused() ? 'P' : 'U', now);
            nl_pause_time();
            now = nl_get_time_native();
            NL_LOG_CRIT(lrTEST, "[%c, B, %u] about to sleep for %ums\n", nl_is_time_paused() ? 'P' : 'U', now, sleep_time_ms);
            nl_task_sleep_ms(sleep_time_ms);
        }
    }
}

#endif


int nl_test_timer_eventhandler(nl_event_t *aEvent, void *aClosure);

nl_event_timer_t timerev1 =
{
    NL_INIT_EVENT_STATIC(NL_EVENT_T_TIMER, nl_test_timer_eventhandler, NULL),
    NULL, 0, 0, 0
};

int nl_test_timer_eventhandler(nl_event_t *aEvent, void *aClosure)
{
    (void)aEvent;
    struct taskAData *data = (struct taskAData *)aClosure;
    NL_LOG_CRIT(lrTEST, "[%c, %s, %u] timeout\n", nl_is_time_paused() ? 'P' : 'U', nl_task_get_current()->mName, nl_get_time_native());

    // Repost same timer event
    nl_init_event_timer(&timerev1, 500);
    nl_eventqueue_post_event(data->mTimer, (nl_event_t *)&timerev1);

    return 0;
}

int nl_test_default_handler(nl_event_t *aEvent, void *aClosure)
{
    (void)aClosure;
    NL_LOG_CRIT(lrTEST, "'%s' got event type: %d -- unexpected\n",
            nl_task_get_current()->mName, aEvent->mType);

    return 0;
}

void taskEntryA(void *aParams)
{
    nl_task_t *curtask = nl_task_get_current();
    struct taskAData *data = (struct taskAData *)aParams;

    NL_LOG_CRIT(lrTEST, "from the task: %s (queue: %08x)\n",
            curtask->mName, data->mQueue);

    timerev1.mHandlerClosure = data;
    timerev1.mReturnQueue = data->mQueue;

    nl_init_event_timer(&timerev1, 500);
    nl_eventqueue_post_event(data->mTimer, (nl_event_t *)&timerev1);

#if NLER_FEATURE_SIMULATEABLE_TIME
    nl_task_create(taskEntryB, "B", stackB, sizeof(stackB), NLER_TASK_PRIORITY_NORMAL, NULL, &taskB);
#endif

    while (1)
    {
        nl_event_t  *ev;

        ev = nl_eventqueue_get_event(data->mQueue);

        nl_dispatch_event(ev, nl_test_default_handler, NULL);
    }
}

int main(int argc, char **argv)
{
    struct taskAData    dataA;
    nl_event_t          *queuememA[50];

    NL_LOG_CRIT(lrTEST, "start main\n");

    nl_er_init();
#if NLER_FEATURE_SIMULATEABLE_TIME
    if (argc == 2 && (strcmp(argv[1], "--pause-time") == 0))
    {
        nl_time_init_sim(true);
        NL_LOG_CRIT(lrTEST, "Starting simulated time paused\n");
    }
    else
    {
        nl_time_init_sim(false);
        NL_LOG_CRIT(lrTEST, "Starting simulated time unpaused\n");
    }
#endif

    NL_LOG_CRIT(lrTEST, "start main (after initializing runtime)\n");

    dataA.mTimer = nl_timer_start(NLER_TASK_PRIORITY_HIGH + 1);
    dataA.mQueue = nl_eventqueue_create(queuememA, sizeof(queuememA));

    nl_task_create(taskEntryA, "A", stackA, sizeof(stackA), NLER_TASK_PRIORITY_NORMAL, &dataA, &taskA);

    nl_er_start_running();

    nl_er_cleanup();

    NL_LOG_CRIT(lrTEST, "end main\n");

    return 0;
}

