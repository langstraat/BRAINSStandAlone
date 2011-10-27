/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: CannyEdgeDetectionImageFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2009-03-16 21:52:48 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined( _MSC_VER )
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

//  Software Guide : BeginLatex
//
//  This example introduces the use of the
//  \doxygen{CannyEdgeDetectionImageFilter}. This filter is widely used for
//  edge detection since it is the optimal solution satisfying the constraints
//  of good sensitivity, localization and noise robustness.
//
//  \index{itk::CannyEdgeDetectionImageFilter|textbf}
//
//  Software Guide : EndLatex

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

//  Software Guide : BeginLatex
//
//  The first step required for using this filter is to include its header file
//
//  \index{itk::CannyEdgeDetectionImageFilter!header}
//
//  Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "itkCannyEdgeDetectionImageFilter.h"
// Software Guide : EndCodeSnippet

#include "CannyEdgeCLP.h"

int main(int argc, char *argv[])
{
  PARSE_ARGS;
  // float variance = 2.0;
  // float upperThreshold = 0.0;
  // float lowerThreshold = 0.0;
  std::cout << "Variance = " << variance << std::endl;
  std::cout << "UpperThreshold = " << upperThreshold << std::endl;
  std::cout << "LowerThreshold = " << lowerThreshold << std::endl;

  typedef float CharPixelType;            //  IO
  typedef float RealPixelType;            //  Operations
  const unsigned int Dimension = 3;

  typedef itk::Image<CharPixelType, Dimension> CharImageType;
  typedef itk::Image<RealPixelType, Dimension> RealImageType;

  typedef itk::ImageFileReader<CharImageType> ReaderType;
  typedef itk::ImageFileWriter<CharImageType> WriterType;

  //  Software Guide : BeginLatex
  //
  //  This filter operates on image of pixel type float. It is then necessary
  //  to cast the type of the input images that are usually of integer type.
  //  The \doxygen{CastImageFilter} is used here for that purpose. Its image
  //  template parameters are defined for casting from the input type to the
  //  float type using for processing.
  //
  //  Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef itk::CastImageFilter<CharImageType, RealImageType> CastToRealFilterType;
  // Software Guide : EndCodeSnippet

  typedef itk::RescaleIntensityImageFilter<RealImageType, CharImageType> RescaleFilter;

  //  Software Guide : BeginLatex
  //
  //  The \doxygen{CannyEdgeDetectionImageFilter} is instantiated using the
  // float image type.
  //
  //  \index{itk::CannyEdgeDetectionImageFilter|textbf}
  //
  //  Software Guide : EndLatex

  typedef itk::CannyEdgeDetectionImageFilter<RealImageType, RealImageType> CannyFilter;

  // Setting the IO

  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  CastToRealFilterType::Pointer toReal = CastToRealFilterType::New();
  RescaleFilter::Pointer        rescale = RescaleFilter::New();

  // Setting the ITK pipeline filter

  CannyFilter::Pointer cannyFilter = CannyFilter::New();

  reader->SetFileName(inputVolume);
  writer->SetFileName(outputVolume);

  // The output of an edge filter is 0 or 1
  rescale->SetOutputMinimum(0);
  rescale->SetOutputMaximum(255);

  toReal->SetInput( reader->GetOutput() );

  cannyFilter->SetInput( toReal->GetOutput() );
  cannyFilter->SetVariance(variance);
  cannyFilter->SetUpperThreshold(upperThreshold);
  cannyFilter->SetLowerThreshold(lowerThreshold);

  rescale->SetInput( cannyFilter->GetOutput() );
  writer->SetInput( rescale->GetOutput() );
  writer->UseCompressionOn();

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "ExceptionObject caught !" << std::endl;
    std::cout << err << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}