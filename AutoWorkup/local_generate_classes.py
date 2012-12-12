## \author Hans J. Johnson
## This file contains the code necessary to build the python module
## nodes for SEM compliant tools
##

## NOTES ON HOW TO RUN THIS.
# build wiht all options turned on (even the non-default options)
# export PATH=~/src/BSA-clang31/bin:${PATH}
# export PYTHONPATH=~/src/BSA-clang31/NIPYPE
#cd ~/src/BRAINSStandAlone/AutoWorkup/SEMTools/; rm -rf ~/src/BRAINSStandAlone/AutoWorkup/SEMTools/* ; python ../local_generate_classes.py

from nipype.interfaces.slicer.generate_classes import generate_all_classes

modules_list = [
  'AssignArray',
  'AverageBrainGenerator',
  'AverageScalarsByResampling',
  'BRAINSABC',
  'BRAINSAlignMSP',
  'BRAINSApplySurfaceLabels',
  'BRAINSAssignSurfaceFeatures',
  'BRAINSClipInferior',
  'BRAINSConstellationDetector',
  'BRAINSConstellationModeler',
  'BRAINSContinuousClass',
  'BRAINSCreateLabelMapFromProbabilityMaps',
  'BRAINSCut',
  'BRAINSDemonWarp',
  'BRAINSEyeDetector',
  'BRAINSFit',
  'BRAINSFitEZ',
  'BRAINSInitializedControlPoints',
  'BRAINSLandmarkInitializer',
  'BRAINSLinearModelerEPCA',
  'BRAINSLmkTransform',
  'BRAINSMeasureSurface',
  'BRAINSMultiModeSegment',
  'BRAINSMush',
  'BRAINSPosteriorToContinuousClass',
  'BRAINSROIAuto',
  'BRAINSResample',
  'BRAINSResize',
  'BRAINSSnapShotWriter',
  'BRAINSSurfaceFlattening',
  'BRAINSSurfaceGeneration',
  'BRAINSTransformConvert',
  'BRAINSTransformFromFiducials',
  'BRAINSTrimForegroundInDirection',
  'BinaryMaskEditorBasedOnLandmarks',
  'CannyEdge',
  'CannySegmentationLevelSetImageFilter',
  'CleanUpOverlapLabels',
  'CombineLabels',
  'CompareSurfaces',
  'CreateMask',
  'DWIConvert',
  'DilateImage',
  'DilateMask',
  'DistanceMaps',
  'ESLR',
  'ErodeImage',
  'FlippedDifference',
  'GenerateBrainClippedImage',
  'GenerateCsfClippedFromClassifiedImage',
  'GenerateLabelMapFromProbabilityMap',
  'GenerateSummedGradientImage',
  'GenerateTestImage',
  'GradientAnisotropicDiffusionImageFilter',
  'HammerAttributeCreator',
  'HistogramMatchingFilter',
  'IcosahedronResampler',
  'ImageRegionPlotter',
  'JointHistogram',
  'LabelMaps',
  'MultiResolutionRegistration',
  'NeighborhoodMean',
  'NeighborhoodMedian',
  'NoiseGenerator',
  'ProbabilityLabels',
  'QuadEdgeMeshClampScalars',
  'QuadEdgeMeshHistogramMatching',
  'QuadEdgeMeshPiecewiseRescale',
  'QuadEdgeMeshSimilarity',
  'RearrangeSurfaceLabels',
  'RemoveTinyLabels',
  'ResampleQuadEdgeMesh',
  'STAPLEAnalysis',
  'ShuffleVectorsModule',
  'SimilarityIndex',
  'SurfaceColor',
  'SurfaceLabelCleanUp',
  'TextureFromNoiseImageFilter',
  'TextureMeasureFilter',
  'VBRAINSDemonWarp',
  'WarpQuadEdgeMesh',
  'compareTractInclusion',
  'extractNrrdVectorIndex',
  'fcsv_to_hdf5',
  'gtractAnisotropyMap',
  'gtractAverageBvalues',
  'gtractClipAnisotropy',
  'gtractCoRegAnatomy',
  'gtractConcatDwi',
  'gtractCopyImageOrientation',
  'gtractCoregBvalues',
  'gtractCostFastMarching',
  'gtractCreateGuideFiber',
  'gtractFastMarchingTracking',
  'gtractFiberTracking',
  'gtractImageConformity',
  'gtractInvertBSplineTransform',
  'gtractInvertDisplacementField',
  'gtractInvertRigidTransform',
  'gtractResampleAnisotropy',
  'gtractResampleB0',
  'gtractResampleCodeImage',
  'gtractResampleDWIInPlace',
  'gtractResampleFibers',
  'gtractTensor',
  'gtractTransformToDisplacementField',
  'iccdefRegistration_New',
  'insertMidACPCpoint',
  'landmarksConstellationAligner',
  'landmarksConstellationWeights',
  'SmoothingMeshScalars'
  ]

launcher=['']
generate_all_classes(modules_list=modules_list, launcher=[])
