/*!
 * \file    chronometer.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The Chronometer class is used to simplify time measurement.
 * 
 * \version 1.0.0
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without notice
 * and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility for any errors
 * that may appear in this document.
 *
 * This document, or source code, is provided solely as an example of how to utilize
 * IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#include "chronometer.h"

#include <chrono>


Chronometer::Chronometer()
{
    m_start = std::chrono::steady_clock::now();
}


Chronometer::~Chronometer()
{}


void Chronometer::Start()
{
    m_start = std::chrono::steady_clock::now();
}


double Chronometer::GetTimeSinceStart_ms()
{
    m_diff = std::chrono::steady_clock::now() - m_start;
    return (m_diff.count() * 1000.0);
}
