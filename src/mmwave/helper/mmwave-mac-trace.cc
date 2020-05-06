/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2020, University of Padova, Dep. of Information Engineering, SIGNET lab
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation;
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   Author: Matteo Pagin <mattpagg@gmail.com>
*/




#include <ns3/log.h>
#include "mmwave-mac-trace.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MmWaveMacTrace");

namespace mmwave {

NS_OBJECT_ENSURE_REGISTERED (MmWaveMacTrace);

std::ofstream MmWaveMacTrace::m_schedAllocTraceFile {};
std::string MmWaveMacTrace::m_schedAllocTraceFilename {};

MmWaveMacTrace::MmWaveMacTrace ()
{
}

MmWaveMacTrace::~MmWaveMacTrace ()
{
  if (m_schedAllocTraceFile.is_open ())
    {
      m_schedAllocTraceFile.close ();
    }
}

TypeId
MmWaveMacTrace::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MmWaveMacTrace")
    .SetParent<Object> ()
    .AddConstructor<MmWaveMacTrace> ()
    .AddAttribute ("SchedInfoOutputFilename",
                   "Name of the file where the allocation info provided by the scheduler will be saved.",
                   StringValue ("EnbSchedAllocTraces.txt"),
                   MakeStringAccessor (&MmWaveMacTrace::SetOutputFilename),
                   MakeStringChecker ())
  ;
  return tid;
}

void
MmWaveMacTrace::ReportEnbSchedulingInfo (Ptr<MmWaveMacTrace> enbStats, MmWaveEnbMac::MmWaveSchedTraceInfo schedParams)
{
    // Open the output file if it is not open yet
    if (!m_schedAllocTraceFile.is_open ())
    {
      m_schedAllocTraceFile.open (m_schedAllocTraceFilename.c_str ());
      if (!m_schedAllocTraceFile.is_open ())
        {
          NS_FATAL_ERROR ("Could not open tracefile");
        }
      m_schedAllocTraceFile << "frame\tsubF\tslot\trnti\tfirstSym\tnumSym\ttype\ttddMode" << std::endl;
    }
    

    SlotAllocInfo allocInfo = schedParams.m_indParam.m_slotAllocInfo;
    SfnSf dlSfn = schedParams.m_indParam.m_sfnSf;   // Holds the intended slot, subframe and frame info for DL data and controls allocations
    SfnSf ulSfn = schedParams.m_ulSfnSf;   // Holds the intended slot, subframe and frame info for UL data and controls allocations


    for (auto iTti : allocInfo.m_ttiAllocInfo)
    {
      if (iTti.m_ttiType == TtiAllocInfo::DATA && iTti.m_tddMode == TtiAllocInfo::UL_slotAllocInfo)   // If UL data, shift by UlSchedDelay
      {
        // Trace the UL DATA alloc info
        m_schedAllocTraceFile << (unsigned)ulSfn.m_frameNum << "\t" << (unsigned)ulSfn.m_sfNum << "\t"
                              << (unsigned)ulSfn.m_slotNum << "\t" << (unsigned)iTti.m_dci.m_rnti << "\t" 
                              << (unsigned)iTti.m_dci.m_symStart << "\t" << (unsigned)iTti.m_dci.m_numSym 
                              << "\t" << iTti.m_ttiType << "\t" << iTti.m_tddMode << std::endl;
      }
      else
      {
        // Trace the DL DATA or CONTROL alloc info
        m_schedAllocTraceFile << (unsigned)dlSfn.m_frameNum << "\t" << (unsigned)dlSfn.m_sfNum << "\t"
                              << (unsigned)dlSfn.m_slotNum << "\t" << (unsigned)iTti.m_dci.m_rnti 
                              << "\t" << (unsigned)iTti.m_dci.m_symStart << "\t" << (unsigned)iTti.m_dci.m_numSym 
                              << "\t" << iTti.m_ttiType << "\t" << iTti.m_tddMode << std::endl;
      }
    }   
}

void
MmWaveMacTrace::SetOutputFilename (std::string fileName)
{
  NS_LOG_INFO ("Filename: " << fileName);
  m_schedAllocTraceFilename = fileName;
}

} // namespace mmwave

} /* namespace ns3 */