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
/**   Internet Protocol (IP)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define NX_SOURCE_CODE


/* Include necessary system files.  */

#include "nx_api.h"
#include "nx_ip.h"
#include "nx_packet.h"


/* Bring in externs for caller checking code.  */

NX_CALLER_CHECKING_EXTERNS


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _nxe_ip_raw_packet_send                             PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Yuxin Zhou, Microsoft Corporation                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function checks for errors in the IP raw packet send           */
/*    function call.                                                      */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    ip_ptr                                Pointer to IP control block   */
/*    packet_ptr                            Pointer to packet to send     */
/*    destination_ip                        Destination IP address        */
/*    type_of_service                       Type of service for packet    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    status                                Completion status             */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    _nx_ip_raw_packet_send                Actual IP raw packet send     */
/*                                            function                    */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    Application                                                         */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Yuxin Zhou               Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/
UINT  _nxe_ip_raw_packet_send(NX_IP *ip_ptr, NX_PACKET **packet_ptr_ptr,
                              ULONG destination_ip, ULONG type_of_service)
{


UINT       status;
NX_PACKET *packet_ptr;


    /* Setup packet pointer.  */
    packet_ptr =  *packet_ptr_ptr;

    /* Check for invalid input pointers.  */
    if ((ip_ptr == NX_NULL) || (ip_ptr -> nx_ip_id != NX_IP_ID) || (packet_ptr == NX_NULL) || (packet_ptr -> nx_packet_tcp_queue_next != ((NX_PACKET *)NX_PACKET_ALLOCATED)))
    {
        return(NX_PTR_ERROR);
    }

    /* Check to see if IP raw packet processing is enabled.  */
    if (!ip_ptr -> nx_ip_raw_ip_processing)
    {
        return(NX_NOT_ENABLED);
    }

    /* Check for invalid IP address.  */
    if (!destination_ip)
    {
        return(NX_IP_ADDRESS_ERROR);
    }

    /* Check for valid type of service.  */
    if (type_of_service & ~(NX_IP_TOS_MASK))
    {
        return(NX_OPTION_ERROR);
    }

    /* Check for an invalid packet prepend pointer.  */
    if ((packet_ptr -> nx_packet_prepend_ptr - sizeof(NX_IP_HEADER)) < packet_ptr -> nx_packet_data_start)
    {
        return(NX_UNDERFLOW);
    }

    /* Check for an invalid packet append pointer.  */
    if (packet_ptr -> nx_packet_append_ptr > packet_ptr -> nx_packet_data_end)
    {
        return(NX_OVERFLOW);
    }

    /* Check for appropriate caller.  */
    NX_THREADS_ONLY_CALLER_CHECKING

    /* Call actual IP raw packet send function.  */
    status =  _nx_ip_raw_packet_send(ip_ptr, packet_ptr, destination_ip, type_of_service);

    /* Determine if the raw packet send was successful.  */
    if (status == NX_SUCCESS)
    {

        /* Yes, now clear the application's packet pointer so it can't be accidentally
           used again by the application.  This is only done when error checking is
           enabled.  */
        *packet_ptr_ptr =  NX_NULL;
    }

    /* Return completion status.  */
    return(status);
}

