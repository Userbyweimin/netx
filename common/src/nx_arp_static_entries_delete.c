/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** NetX Component                                                        */
/**                                                                       */
/**   Address Resolution Protocol (ARP)                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "nx_arp.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nx_arp_static_entries_delete                       PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function deletes all ARP static entries currently in           */
/*    the ARP cache.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                IP instance pointer           */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    tx_mutex_get                          Obtain protection mutex       */
/*    tx_mutex_put                          Release protection mutex      */
/*    _nx_arp_static_entry_delete           Delete a static entry         */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application Code                                                    */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
UINT  _nx_arp_static_entries_delete(NX_IP *ip_ptr)
{

NX_ARP *arp_entry;
UINT    status;

#ifdef TX_ENABLE_EVENT_TRACE
TX_TRACE_BUFFER_ENTRY *trace_event;
ULONG                  trace_timestamp;
ULONG                  deleted_count =  0;
#endif


    /* If trace is enabled, insert this event into the trace buffer.  */
    NX_TRACE_IN_LINE_INSERT(NX_TRACE_ARP_STATIC_ENTRIES_DELETE, ip_ptr, 0, 0, 0, NX_TRACE_ARP_EVENTS, &trace_event, &trace_timestamp)

    /* Obtain protection on this IP instance for access into the ARP static
       list.  */
    tx_mutex_get(&(ip_ptr -> nx_ip_protection), TX_WAIT_FOREVER);

    /* Traverse the static list until it is exhausted.  */
    do
    {

        /* Pickup the head of the list.  */
        arp_entry =  ip_ptr -> nx_ip_arp_static_list;

        /* Determine if the list is exhausted.  */
        if (arp_entry == NX_NULL)
        {
            break;
        }

        /* Otherwise, delete the static entry delete routine.  Note that the delete routine
           will modify the list head pointer used above. */
        status =  _nx_arp_static_entry_delete(ip_ptr, arp_entry -> nx_arp_ip_address,
                                              arp_entry -> nx_arp_physical_address_msw,
                                              arp_entry -> nx_arp_physical_address_lsw);

#ifdef TX_ENABLE_EVENT_TRACE

        /* Increment the deleted count.  */
        deleted_count++;
#endif
    } while (status == NX_SUCCESS);

    /* Update the trace event with the status.  */
    NX_TRACE_EVENT_UPDATE(trace_event, trace_timestamp, NX_TRACE_ARP_STATIC_ENTRIES_DELETE, 0, deleted_count, 0, 0)

    /* Release the mutex.  */
    tx_mutex_put(&(ip_ptr -> nx_ip_protection));

    /* Return status to the caller.  */
    return(NX_SUCCESS);
}

