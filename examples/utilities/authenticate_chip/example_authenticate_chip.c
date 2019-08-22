/**
* MIT License
*
* Copyright (c) 2019 Infineon Technologies AG
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE
*
*
* \file example_optiga_crypt_ecdsa_sign.c
*
* \brief   This file provides the example for ECDSA Sign operation using #optiga_crypt_ecdsa_sign.
* 
*
* \ingroup
* @{
*/

#include "optiga/optiga_crypt.h"
#include "optiga/optiga_util.h"
#include "optiga/common/optiga_lib_common.h"
#include "pal_crypt.h"

/*************************************************************************
*  Global
*************************************************************************/
///size of public key for NIST-P256
#define LENGTH_PUB_KEY_NISTP256     0x41

///Length of R and S vector
#define LENGTH_RS_VECTOR            0x40

///Length of maximum additional bytes to encode sign in DER
#define MAXLENGTH_SIGN_ENCODE       0x06

///Length of Signature
#define LENGTH_SIGNATURE            (LENGTH_RS_VECTOR + MAXLENGTH_SIGN_ENCODE)

// Length of the requested challenge
#define LENGTH_CHALLENGE			32

// Length of SH256
#define LENGTH_SHA256			    32

///size of end entity certificate of OPTIGA™ Trust M
#define LENGTH_OPTIGA_CERT          512

#define MODULE_ENABLE_ONE_WAY_AUTH

#ifdef MODULE_ENABLE_ONE_WAY_AUTH

#ifndef TRUSTX_USE_TEST_CA
// OPTIGA™ Trust X Root CA. Hexadeciaml representation of the "Infineon OPTIGA(TM) Trust X CA 101" certificate
uint8_t optiga_ca_trustx_certificate[] = {
		0x30, 0x82, 0x02, 0x78, 0x30, 0x82, 0x01, 0xfe, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x04, 0x6a,
		0xdb, 0xdd, 0xd6, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03, 0x30,
		0x77, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x21,
		0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x66, 0x69, 0x6e, 0x65, 0x6f,
		0x6e, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6e, 0x6f, 0x6c, 0x6f, 0x67, 0x69, 0x65, 0x73, 0x20, 0x41,
		0x47, 0x31, 0x1b, 0x30, 0x19, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x12, 0x4f, 0x50, 0x54, 0x49,
		0x47, 0x41, 0x28, 0x54, 0x4d, 0x29, 0x20, 0x44, 0x65, 0x76, 0x69, 0x63, 0x65, 0x73, 0x31, 0x28,
		0x30, 0x26, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x1f, 0x49, 0x6e, 0x66, 0x69, 0x6e, 0x65, 0x6f,
		0x6e, 0x20, 0x4f, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4d, 0x29, 0x20, 0x45, 0x43, 0x43,
		0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x37, 0x30, 0x38,
		0x32, 0x39, 0x31, 0x36, 0x32, 0x37, 0x30, 0x38, 0x5a, 0x17, 0x0d, 0x34, 0x32, 0x30, 0x38, 0x32,
		0x39, 0x31, 0x36, 0x32, 0x37, 0x30, 0x38, 0x5a, 0x30, 0x72, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03,
		0x55, 0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a,
		0x0c, 0x18, 0x49, 0x6e, 0x66, 0x69, 0x6e, 0x65, 0x6f, 0x6e, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6e,
		0x6f, 0x6c, 0x6f, 0x67, 0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03,
		0x55, 0x04, 0x0b, 0x0c, 0x0a, 0x4f, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4d, 0x29, 0x31,
		0x2b, 0x30, 0x29, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x22, 0x49, 0x6e, 0x66, 0x69, 0x6e, 0x65,
		0x6f, 0x6e, 0x20, 0x4f, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4d, 0x29, 0x20, 0x54, 0x72,
		0x75, 0x73, 0x74, 0x20, 0x58, 0x20, 0x43, 0x41, 0x20, 0x31, 0x30, 0x31, 0x30, 0x59, 0x30, 0x13,
		0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
		0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x60, 0xd7, 0x9d, 0x39, 0x60, 0xfb, 0x10, 0xd4, 0x28,
		0x89, 0x09, 0x56, 0x4f, 0xfd, 0xa8, 0x47, 0xe2, 0x22, 0xfd, 0x8d, 0x3a, 0x24, 0x07, 0x7b, 0x38,
		0x0d, 0xc3, 0x70, 0x4e, 0x37, 0x42, 0x08, 0x1b, 0x33, 0xc6, 0xec, 0x47, 0xd0, 0xa8, 0xfb, 0xcf,
		0xad, 0x3f, 0xdc, 0x7c, 0x6e, 0xcd, 0x94, 0x7a, 0x4c, 0x1e, 0x90, 0x63, 0xd0, 0x7f, 0xe4, 0x20,
		0xa7, 0xab, 0x14, 0xd5, 0x92, 0xb6, 0xc0, 0xa3, 0x7d, 0x30, 0x7b, 0x30, 0x1d, 0x06, 0x03, 0x55,
		0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xca, 0x05, 0x33, 0xd7, 0x4f, 0xc4, 0x7f, 0x09, 0x49, 0xfb,
		0xdb, 0x12, 0x25, 0xdf, 0xd7, 0x97, 0x9d, 0x41, 0x1e, 0x15, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d,
		0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x00, 0x04, 0x30, 0x12, 0x06, 0x03, 0x55, 0x1d,
		0x13, 0x01, 0x01, 0xff, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01, 0xff, 0x02, 0x01, 0x00, 0x30, 0x15,
		0x06, 0x03, 0x55, 0x1d, 0x20, 0x04, 0x0e, 0x30, 0x0c, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x82, 0x14,
		0x00, 0x44, 0x01, 0x14, 0x01, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16,
		0x80, 0x14, 0xb4, 0x18, 0x85, 0xc8, 0x4a, 0x4a, 0xc5, 0x12, 0x7a, 0xf2, 0x40, 0x39, 0xde, 0xc4,
		0xf5, 0x8b, 0x1e, 0x7e, 0x4a, 0xd1, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04,
		0x03, 0x03, 0x03, 0x68, 0x00, 0x30, 0x65, 0x02, 0x31, 0x00, 0xd2, 0x21, 0x49, 0xc3, 0x46, 0x70,
		0x4b, 0x16, 0x85, 0x9e, 0xf2, 0x92, 0x6d, 0x0c, 0xd2, 0xb8, 0x74, 0x4f, 0xdd, 0x12, 0x61, 0x78,
		0x45, 0x9b, 0x54, 0x31, 0xd2, 0x9d, 0x50, 0x4a, 0xdd, 0x5c, 0xfe, 0xf7, 0x54, 0x12, 0xb8, 0x03,
		0xc2, 0x11, 0x21, 0x95, 0x53, 0xfc, 0x30, 0x39, 0x00, 0xd6, 0x02, 0x30, 0x13, 0x62, 0x98, 0x1f,
		0xe7, 0x64, 0x4c, 0x89, 0xef, 0xf0, 0xe7, 0x83, 0xeb, 0x71, 0x5c, 0xa1, 0xae, 0x47, 0xf7, 0xe7,
		0xfb, 0x7e, 0x70, 0xa8, 0xdf, 0x28, 0x04, 0x14, 0x42, 0x47, 0x66, 0x70, 0x62, 0x22, 0x1d, 0xbf,
		0xf3, 0xe6, 0xb3, 0x5e, 0x23, 0xcb, 0x29, 0x32, 0xde, 0xea, 0xb5, 0x8e
};
#else
//Infineon Test CA Certificate. Used only for Testing a verification
uint8_t optiga_ca_certificate[]=
{
        0x30, 0x82, 0x02, 0x62, 0x30, 0x82, 0x02, 0x08, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00,
        0xC6, 0x40, 0x14, 0x6A, 0x1D, 0xDA, 0xFE, 0x46, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE,
        0x3D, 0x04, 0x03, 0x02, 0x30, 0x77, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
        0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x18, 0x49, 0x6E,
        0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6E, 0x6F, 0x6C, 0x6F, 0x67,
        0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C,
        0x0A, 0x4F, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4D, 0x29, 0x31, 0x30, 0x30, 0x2E, 0x06,
        0x03, 0x55, 0x04, 0x03, 0x0C, 0x27, 0x49, 0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x4F,
        0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4D, 0x29, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20,
        0x58, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x43, 0x41, 0x20, 0x30, 0x30, 0x30, 0x30, 0x1E, 0x17,
        0x0D, 0x31, 0x36, 0x30, 0x35, 0x31, 0x30, 0x32, 0x30, 0x31, 0x38, 0x33, 0x30, 0x5A, 0x17, 0x0D,
        0x34, 0x31, 0x30, 0x35, 0x30, 0x34, 0x32, 0x30, 0x31, 0x38, 0x33, 0x30, 0x5A, 0x30, 0x77, 0x31,
        0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1F,
        0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x18, 0x49, 0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20,
        0x54, 0x65, 0x63, 0x68, 0x6E, 0x6F, 0x6C, 0x6F, 0x67, 0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31,
        0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x0A, 0x4F, 0x50, 0x54, 0x49, 0x47, 0x41,
        0x28, 0x54, 0x4D, 0x29, 0x31, 0x30, 0x30, 0x2E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x27, 0x49,
        0x6E, 0x66, 0x69, 0x6E, 0x65, 0x6F, 0x6E, 0x20, 0x4F, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54,
        0x4D, 0x29, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x58, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20,
        0x43, 0x41, 0x20, 0x30, 0x30, 0x30, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
        0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00,
        0x04, 0x94, 0x89, 0x2F, 0x09, 0xEA, 0x4E, 0xCA, 0xBC, 0x6A, 0x4E, 0xF2, 0x06, 0x36, 0x26, 0xE0,
        0x5D, 0xE0, 0xD5, 0xF9, 0x77, 0xEA, 0xC3, 0xB2, 0x70, 0xAC, 0xE2, 0x19, 0x00, 0xF5, 0xDB, 0x56,
        0xE7, 0x37, 0xBB, 0xBE, 0x46, 0xE4, 0x49, 0x76, 0x38, 0x25, 0xB5, 0xF8, 0x94, 0x74, 0x9E, 0x1A,
        0xB6, 0x5A, 0xF1, 0x29, 0xD7, 0x3A, 0xB6, 0x9B, 0x80, 0xAC, 0xC5, 0xE1, 0xC3, 0x10, 0xF2, 0x16,
        0xC6, 0xA3, 0x7D, 0x30, 0x7B, 0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14,
        0x42, 0xE3, 0x5D, 0x56, 0xE5, 0x6C, 0x8E, 0x8D, 0x02, 0x71, 0x8C, 0x9E, 0xF2, 0x33, 0xC9, 0x47,
        0x3B, 0x82, 0x53, 0x6C, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80,
        0x14, 0x42, 0xE3, 0x5D, 0x56, 0xE5, 0x6C, 0x8E, 0x8D, 0x02, 0x71, 0x8C, 0x9E, 0xF2, 0x33, 0xC9,
        0x47, 0x3B, 0x82, 0x53, 0x6C, 0x30, 0x12, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x01, 0x01, 0xFF, 0x04,
        0x08, 0x30, 0x06, 0x01, 0x01, 0xFF, 0x02, 0x01, 0x00, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x1D, 0x0F,
        0x01, 0x01, 0xFF, 0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x15, 0x06, 0x03, 0x55, 0x1D, 0x20,
        0x04, 0x0E, 0x30, 0x0C, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x82, 0x14, 0x00, 0x44, 0x01, 0x14, 0x01,
        0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30,
        0x45, 0x02, 0x21, 0x00, 0xF5, 0xF3, 0x2B, 0x5B, 0x93, 0x09, 0x92, 0x90, 0x2C, 0xA4, 0x5F, 0x74,
        0x56, 0xC1, 0x24, 0xBB, 0x2B, 0x9C, 0xE4, 0x4F, 0xC7, 0xF0, 0xF1, 0x6C, 0x3F, 0x5F, 0x81, 0x53,
        0x9F, 0x09, 0x77, 0x98, 0x02, 0x20, 0x51, 0xB0, 0x82, 0x77, 0x85, 0x06, 0x77, 0xDE, 0xEF, 0x3D,
        0x49, 0x21, 0xB7, 0x92, 0x1D, 0x87, 0xB5, 0xC2, 0x92, 0x6D, 0x91, 0x07, 0x9D, 0x02, 0xEA, 0x63,
        0x1C, 0xA8, 0xE9, 0x91, 0x25, 0xA6,
};
#endif

// OPTIGA™ Trust M Root CA. Hexadeciaml representation of the "Infineon OPTIGA(TM) Trust M CA" certificate
const uint8_t optiga_ca_certificate[] = {
        0x30, 0x82, 0x02, 0x5f, 0x30, 0x82, 0x02, 0x05, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x09, 0x00,
		0xfb, 0xe1, 0xca, 0x1a, 0x90, 0xf5, 0x20, 0x64, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce,
		0x3d, 0x04, 0x03, 0x02, 0x30, 0x77, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13,
		0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e,
		0x66, 0x69, 0x6e, 0x65, 0x6f, 0x6e, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6e, 0x6f, 0x6c, 0x6f, 0x67,
		0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c,
		0x0a, 0x4f, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4d, 0x29, 0x31, 0x30, 0x30, 0x2e, 0x06,
		0x03, 0x55, 0x04, 0x03, 0x0c, 0x27, 0x49, 0x6e, 0x66, 0x69, 0x6e, 0x65, 0x6f, 0x6e, 0x20, 0x4f,
		0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54, 0x4d, 0x29, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20,
		0x4d, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x43, 0x41, 0x20, 0x30, 0x30, 0x30, 0x30, 0x1e, 0x17,
		0x0d, 0x31, 0x38, 0x30, 0x36, 0x31, 0x35, 0x31, 0x34, 0x32, 0x39, 0x35, 0x33, 0x5a, 0x17, 0x0d,
		0x34, 0x33, 0x30, 0x36, 0x30, 0x39, 0x31, 0x34, 0x32, 0x39, 0x35, 0x33, 0x5a, 0x30, 0x77, 0x31,
		0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x44, 0x45, 0x31, 0x21, 0x30, 0x1f,
		0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x66, 0x69, 0x6e, 0x65, 0x6f, 0x6e, 0x20,
		0x54, 0x65, 0x63, 0x68, 0x6e, 0x6f, 0x6c, 0x6f, 0x67, 0x69, 0x65, 0x73, 0x20, 0x41, 0x47, 0x31,
		0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c, 0x0a, 0x4f, 0x50, 0x54, 0x49, 0x47, 0x41,
		0x28, 0x54, 0x4d, 0x29, 0x31, 0x30, 0x30, 0x2e, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x27, 0x49,
		0x6e, 0x66, 0x69, 0x6e, 0x65, 0x6f, 0x6e, 0x20, 0x4f, 0x50, 0x54, 0x49, 0x47, 0x41, 0x28, 0x54,
		0x4d, 0x29, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x4d, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20,
		0x43, 0x41, 0x20, 0x30, 0x30, 0x30, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce,
		0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00,
		0x04, 0x1b, 0x51, 0xfd, 0xac, 0x28, 0xa5, 0xbd, 0x0b, 0x39, 0x57, 0x41, 0xa7, 0x00, 0x6e, 0x23,
		0x64, 0xf8, 0xd3, 0xc4, 0x08, 0xc7, 0x5c, 0xa0, 0x80, 0x5e, 0x35, 0xf6, 0x6e, 0x9f, 0x10, 0x1f,
		0x25, 0x8c, 0x56, 0xf6, 0x21, 0x33, 0xd5, 0xd9, 0x45, 0x2e, 0x5f, 0xa7, 0x70, 0x29, 0xec, 0xf9,
		0x99, 0xb3, 0x4a, 0x73, 0xa5, 0x9b, 0x98, 0xaa, 0x96, 0xf8, 0x0a, 0x35, 0x37, 0x0a, 0x88, 0x8e,
		0x67, 0xa3, 0x7a, 0x30, 0x78, 0x30, 0x12, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff, 0x04,
		0x08, 0x30, 0x06, 0x01, 0x01, 0xff, 0x02, 0x01, 0x00, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f,
		0x04, 0x04, 0x03, 0x02, 0x02, 0x04, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04,
		0x14, 0x53, 0x1b, 0x46, 0x32, 0xf2, 0xba, 0x1b, 0xec, 0x35, 0x23, 0xb0, 0xc6, 0x84, 0xe2, 0xbc,
		0x7f, 0x11, 0xda, 0xa2, 0x2e, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16,
		0x80, 0x14, 0x53, 0x1b, 0x46, 0x32, 0xf2, 0xba, 0x1b, 0xec, 0x35, 0x23, 0xb0, 0xc6, 0x84, 0xe2,
		0xbc, 0x7f, 0x11, 0xda, 0xa2, 0x2e, 0x30, 0x15, 0x06, 0x03, 0x55, 0x1d, 0x20, 0x04, 0x0e, 0x30,
		0x0c, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x82, 0x14, 0x00, 0x44, 0x01, 0x14, 0x01, 0x30, 0x0a, 0x06,
		0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20,
		0x1b, 0xb3, 0x72, 0xa2, 0x3e, 0x36, 0x85, 0xcf, 0x21, 0xa3, 0xe2, 0x95, 0x4f, 0x67, 0x0c, 0x44,
		0x69, 0x45, 0x70, 0xd8, 0xa8, 0x8e, 0x2f, 0x76, 0xb0, 0x5c, 0x0f, 0x5f, 0x27, 0xf2, 0xeb, 0xf1,
		0x02, 0x21, 0x00, 0xad, 0xf0, 0xd3, 0xe1, 0x8b, 0xf2, 0xe2, 0x5f, 0x45, 0x98, 0x48, 0x0c, 0xb6,
		0x43, 0x18, 0x2f, 0xa3, 0x8f, 0xe0, 0x8a, 0x6e, 0xf3, 0xdd, 0x2a, 0xf1, 0xef, 0x7c, 0x27, 0x6a,
		0x44, 0xb6, 0x0f
};


extern optiga_util_t * util_handler;
extern optiga_lib_status_t open_app_status;

static optiga_crypt_t * me_crypt;

//Use the print sparsely
#define PRINT_RANDOM_CHALLENGE   0
#define PRINT_PUBLICKEY          0
#define PRINT_SIGNATURE          0
#define PRINT_SHA                0
#define PRINT_DEVICE_CERT        0

#define USE_RANDOM_CHALLENGE     1 //Either use random challenge or fixed challenge

/**
 * Callback when optiga_crypt_xxxx operation is completed asynchronously
 */

optiga_lib_status_t optiga_lib_crypt_status;
static void optiga_crypt_callback(void * context, optiga_lib_status_t return_status)
{
	optiga_lib_crypt_status = return_status;
    if (NULL != context)
    {
        // callback to upper layer here
    }
}

/**
*
* Read End Device Certificate stored in OPTIGA™ Trust M
*
* \param[in]        chip_cert_oid          Certificate OID
* \param[in]        chip_pubkey	           Pointer to public key buffer
* \param[in,out]    chip_pubkey_size	   Pointer to public key buffer size
*
* \retval    #OPTIGA_LIB_SUCCESS
* \retval    #OPTIGA_LIB_ERROR
*
*/
optiga_lib_status_t read_chip_cert(uint16_t cert_oid,
		                                   uint8_t* p_cert, uint16_t* p_cert_size)
{
	int32_t status  = (int32_t)OPTIGA_DEVICE_ERROR;
	uint8_t tmp_cert[LENGTH_OPTIGA_CERT];
	uint8_t* p_tmp_cert_pointer = tmp_cert;

	printf((">read_chip_cert()\r\n"));

	memset(tmp_cert, 0, LENGTH_OPTIGA_CERT);
	do
	{
		// Sanity check
		if ((NULL == p_cert) || (NULL == p_cert_size) ||
			(0 == cert_oid) || (0 == *p_cert_size) || (LENGTH_OPTIGA_CERT > *p_cert_size))
		{
			break;
		}

		open_app_status = OPTIGA_LIB_BUSY;

		//Read end-entity device certificate
		status = optiga_util_read_data((optiga_util_t *)util_handler, cert_oid, 0, p_tmp_cert_pointer, p_cert_size);
		if(OPTIGA_LIB_SUCCESS != status)
		{
			printf("Error: optiga_util_read_data error status=0x%x\r\n", (unsigned int) status);
			break;
		}

        while (OPTIGA_LIB_BUSY == open_app_status)
        {
            //Wait until the optiga_util_read_data operation is completed
        }

		// Refer to the Solution Reference Manual (SRM) v1.35 Table 30. Certificate Types
		switch (p_tmp_cert_pointer[0])
		{
		/* One-Way Authentication Identity. Certificate DER coded The first byte
		*  of the DER encoded certificate is 0x30 and is used as Tag to differentiate
		*  from other Public Key Certificate formats defined below.
		*/
		case 0x30:
			/* The certificate can be directly used */
			status = OPTIGA_LIB_SUCCESS;
			break;
		/* TLS Identity. Tag = 0xC0; Length = Value length (2 Bytes); Value = Certificate Chain
		 * Format of a "Certificate Structure Message" used in TLS Handshake
		 */
		case 0xC0:
			/* There might be a certificate chain encoded.
			 * For this example we will consider only one certificate in the chain
			 */
			p_tmp_cert_pointer = p_tmp_cert_pointer + 9;
			*p_cert_size = *p_cert_size - 9;
			memcpy(p_cert, p_tmp_cert_pointer, *p_cert_size);
			status = OPTIGA_LIB_SUCCESS;
			break;
		/* USB Type-C identity
		 * Tag = 0xC2; Length = Value length (2 Bytes); Value = USB Type-C Certificate Chain [USB Auth].
		 * Format as defined in Section 3.2 of the USB Type-C Authentication Specification (SRM)
		 */
		case 0xC2:
		// Not supported for this example
		// Certificate type isn't supported or a wrong tag
		default:
			break;
		}

	}while(FALSE);


#if (PRINT_DEVICE_CERT ==1)
        for(int i=0; i<(*p_cert_size);)
        {
        	printf("%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
        	        			p_tmp_cert_pointer[i], p_tmp_cert_pointer[i+1],p_tmp_cert_pointer[i+2],p_tmp_cert_pointer[i+3],
        	        			p_tmp_cert_pointer[i+4], p_tmp_cert_pointer[i+5],p_tmp_cert_pointer[i+6],p_tmp_cert_pointer[i+7],
        	        			p_tmp_cert_pointer[i+8], p_tmp_cert_pointer[i+9],p_tmp_cert_pointer[i+10],p_tmp_cert_pointer[i+11],
        	        			p_tmp_cert_pointer[i+12], p_tmp_cert_pointer[i+13],p_tmp_cert_pointer[i+14],p_tmp_cert_pointer[i+15]);

        	i+=16;
        	if((*p_cert_size)-i<16)
			{
				int x=(*p_cert_size)-i;
				while(x)
				{
					printf("%.2x ",p_tmp_cert_pointer[i]);
					i++;
					x--;
				}
				printf(("\r\n"));
			}
        }
#endif
	return status;
}

/**
*
* Authenticate end device entity using the Challenge-Response.<br>
*
* \param[in]  PpsOPTIGAPublicKey	Public key used to verify the response
* \param[in]  PublicKeySize	        Length of Public key
* \param[in]  PwOPTIGAPrivKey		Private key OID to be used for auth scheme
*
* \retval    #INT_LIB_OK
* \retval    #INT_LIB_ERROR
* \retval    #INT_LIB_MALLOC_FAILURE
*
*/
static optiga_lib_status_t Send_Challenge_Get_Response(uint8_t* p_pubkey, uint16_t pubkey_size, uint16_t privkey_oid)
{
    int32_t status  = OPTIGA_DEVICE_ERROR;
    uint8_t random[LENGTH_CHALLENGE];
    uint8_t signature[LENGTH_SIGNATURE];
    uint16_t signature_size = LENGTH_SIGNATURE;
    uint8_t digest[LENGTH_SHA256];

    printf(">Send_Challenge_Get_Response()\r\n");
    do
    {

    	printf("Seed the Challenge (len=%d)\r\n",LENGTH_CHALLENGE);
        status = pal_crypt_random(LENGTH_CHALLENGE, random);
        if(OPTIGA_LIB_SUCCESS != status)
        {
        	printf(("Error: Random number failed\r\n"));
            break;
        }
#if (PRINT_RANDOM_CHALLENGE==1)
        for(int i=0; i<LENGTH_CHALLENGE;)
        {
        	printf(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n",
        			random[i],random[i+1],random[i+2],random[i+3],
        			random[i+4],random[i+5],random[i+6],random[i+7] ));
        	i+=8;
        	if(LENGTH_CHALLENGE-i<8)
			{
				int x=LENGTH_CHALLENGE-i;
				while(x)
				{
					printf(("%.2x ",random[i]));
					i++;
					x--;
				}
				printf(("\r\n"));
			}
        }
#endif

#if (USE_RANDOM_CHALLENGE==1)
        printf("Hash the Challenge (len=%d)\r\n",LENGTH_CHALLENGE);
        status = pal_crypt_generate_sha256(random, LENGTH_CHALLENGE, digest);
        if(OPTIGA_LIB_SUCCESS != status)
        {
        	printf(("Error: SHA256 failed\r\n"));
        	status = (int32_t)CRYPTO_LIB_VERIFY_SIGN_FAIL;
            break;
        }
        printf("Hash value (len=%d)\r\n",LENGTH_SHA256);
#if (PRINT_SHA==1)
        for(int i=0; i<LENGTH_SHA256;)
        {
        	printf(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n", digest[i],digest[i+1],digest[i+2],digest[i+3],
        			                                               digest[i+4],digest[i+5],digest[i+6],digest[i+7] ));
        	i+=8;
        }
#endif
        printf(("Sign hash using Private Key\r\n"));
#else//Fixed Challenge
        memset(digest, 0x0, LENGTH_SHA256);
        for(int i=0; i<LENGTH_SHA256;)
        {
        	printf(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n", digest[i],digest[i+1],digest[i+2],digest[i+3],
        			                                               digest[i+4],digest[i+5],digest[i+6],digest[i+7] ));
        	i+=8;
        }
#endif


        optiga_lib_crypt_status = OPTIGA_LIB_BUSY;
		//Sign random with OPTIGA™ Trust M
        status = optiga_crypt_ecdsa_sign(me_crypt, digest, LENGTH_SHA256,
									     privkey_oid,
										 signature, &signature_size);
        if (OPTIGA_LIB_SUCCESS != status)
        {
        	printf("Error: Queue sign cmd failed. status=0x%x\r\n",(unsigned int)status);
            break;
        }

        while (OPTIGA_LIB_BUSY == optiga_lib_crypt_status)
        {
            //Wait until the optiga_crypt_ecdsa_sign operation is completed
        }

        if (OPTIGA_LIB_SUCCESS != optiga_lib_crypt_status)
        {
        	printf("Error: Signature failed. status=0x%x\r\n",optiga_lib_crypt_status);
        	status=OPTIGA_CRYPT_ERROR;
            break;
        }

        printf("Signature as Response (len=%d)\r\n", signature_size );
#if (PRINT_SIGNATURE==1)
        for(int i=0; i<signature_size;)
        {
        	printf(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n", signature[i],signature[i+1],signature[i+2],signature[i+3],
        			                                               signature[i+4],signature[i+5],signature[i+6],signature[i+7] ));
        	i+=8;
        	if(signature_size-i<8)
			{
				int x=signature_size-i;
				while(x)
				{
					printf(("%.2x ",signature[i]));
					i++;
					x--;
				}
				printf(("\r\n"));
			}
        }
#endif

        printf("Public Key (len=%d)\r\n", pubkey_size);
#if (PRINT_PUBLICKEY==1)
        for(int i=0; i<pubkey_size;)
        {
        	printf(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x \r\n", p_pubkey[i],p_pubkey[i+1],p_pubkey[i+2],p_pubkey[i+3],
        			                                               p_pubkey[i+4],p_pubkey[i+5],p_pubkey[i+6],p_pubkey[i+7] ));
        	i+=8;
        	if(pubkey_size-i<8)
			{
				int x=pubkey_size-i;
				while(x)
				{
					printf(("%.2x ",p_pubkey[i]));
					i++;
					x--;
				}
				printf(("\r\n"));
			}
        }
#endif
        printf(("Verify Signature using Public Key\r\n"));

		//Verify the signature on the random number by Security Chip
		status = pal_crypt_verify_signature(p_pubkey, pubkey_size,
				                            signature, signature_size,
											digest, LENGTH_SHA256);
		if(OPTIGA_LIB_SUCCESS != status)
		{
			printf(("Result: Authentication failed\r\n"));
			status=OPTIGA_CRYPT_ERROR;
			break;
		}

		printf(("Result: Authentication Ok\r\n"));
		status = OPTIGA_LIB_SUCCESS;
	} while (FALSE);

    printf(("<Send_Challenge_Get_Response()\r\n"));

    return status;
}

/**
 * Example demonstrates the authentication of the Trust M using MbedTLS.
 *
 * Example for #example_authenticate_chip
 *
 */
optiga_lib_status_t example_authenticate_chip(void)
{
    optiga_lib_status_t status;
	uint8_t chip_cert[LENGTH_OPTIGA_CERT];
	uint16_t chip_cert_size = LENGTH_OPTIGA_CERT;
	uint8_t chip_pubkey[LENGTH_PUB_KEY_NISTP256];
	uint16_t chip_pubkey_size = LENGTH_PUB_KEY_NISTP256;
	uint16_t chip_cert_oid = 0xE0E0;
	uint16_t chip_privkey_oid = 0xE0F0;


	printf((">example_authenticate_chip()\r\n"));
    do
    {
    	me_crypt = optiga_crypt_create(0, optiga_crypt_callback, NULL);
    	if (NULL == me_crypt)
    	{
    		printf(("Error: Failed to create crypto instance\r\n"));
    		break;
    	}

    	// Initialise pal crypto module
    	status = pal_crypt_init(me_crypt);

		if(OPTIGA_LIB_SUCCESS != status)
		{
			printf("Error: Crypto init failed (status=0x%x).\r\n", status);
			break;
		}

		printf("Read default cert and verify it\r\n");

		// Read security chip certificate
    	status = read_chip_cert(chip_cert_oid, chip_cert, &chip_cert_size);
		if(OPTIGA_LIB_SUCCESS != status)
		{
			printf("Error: read chip cert failed (status=0x%x).\r\n", status);
			break;
		}

		// Verify the end device certificate against the CA certificate
		status = pal_crypt_verify_certificate(optiga_ca_certificate, sizeof(optiga_ca_certificate), chip_cert, chip_cert_size);
		if(CRYPTO_LIB_OK != status)
		{
			printf("Error: verify cert failed. (status=0x%x)\r\n", status);
			break;
		}

		printf("Extract Public key from cert.\r\n");

		// Extract Public Key from the certificate
		status = pal_crypt_get_public_key(chip_cert, chip_cert_size, chip_pubkey, &chip_pubkey_size);
		if(CRYPTO_LIB_OK != status)
		{
			printf("Error: extract public key failed. (status=0x%x)\r\n", status);
			break;
		}

		printf(("Generate a Challenge using Private key and verify Response using Public Key.\r\n"));
    	status = Send_Challenge_Get_Response(chip_pubkey, chip_pubkey_size, chip_privkey_oid);
		if(OPTIGA_LIB_SUCCESS != status)
		{
			printf("Result: One-way Auth failed. (status=0x%x)\r\n", status);
			break;
		}

		printf(("Result: One-way Auth Ok.\r\n"));

    } while(FALSE);

    //Destroy the crypto handler
    if (me_crypt != NULL)
    		optiga_crypt_destroy(me_crypt);

    printf(("<example_authenticate_chip()\r\n"));

    return status;
}

#endif // MODULE_ENABLE_ONE_WAY_AUTH

/**
* @}
*/
