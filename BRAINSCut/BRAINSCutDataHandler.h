#ifndef BRAINSCutDataHandler_h
#define BRAINSCutDataHandler_h

#include "BRAINSCutUtilities.h"

/*
 * BRAINSCut Primary Class Starts here
 */

class BRAINSCutDataHandler
{
public:
  BRAINSCutDataHandler(){};
  BRAINSCutDataHandler(const std::string modelConfigurationFilenameFilename);

  void                     SetNetConfiguration();
  BRAINSCutConfiguration * GetNetConfiguration()const ;

  void        SetNetConfigurationFilename(const std::string filename);
  std::string GetNetConfigurationFilename() const;

  void SetAtlasDataSet();
  void SetAtlasImage();

  void SetRegionsOfInterest();

  void         SetRegistrationParameters();
  std::string  GetRegistrationID() const;
  void         SetRegistrationImageTypeToUse( std::string type );
  std::string  GetRegistrationImageTypeToUse( ) const;

  void         SetRhoPhiTheta();

  void         SetGradientSize();
  unsigned int GetGradientSize() const;

  void SetTrainingVectorConfiguration();

  std::string GetModelBaseName() const;

  void        SetANNModelFilenameAtIteration( const int iteration);
#if 0
  std::string GetANNModelFilenameAtIteration( const int iteration) const;
#endif
  void        SetANNTestingSSEFilename();
  std::string GetANNTestingSSEFilename() const;
  std::string GetANNModelFilename( ) const;

  void        SetRandomForestModelFilename( int depth, int nTree );
  void        SetRandomForestModelFilename( std::string name );
  std::string GetRandomForestModelFilename() const;
  std::string GetRFModelFilename( const int depth,const int NTrees) const;

  DataSet::StringVectorType GetROIIDsInOrder() const;

  void        SetTrainVectorFilename();
  std::string GetTrainVectorFilename() const;

  void                  GetDeformedSpatialLocationImages(
                                         std::map<std::string, WorkingImagePointer>& warpedSpatialLocationImages,
                                         DataSet& subject ) const;
  void                  GetImagesOfSubjectInOrder( WorkingImageVectorType& subjectImageList, DataSet& subject) const;
  void                  GetDeformedROIs( std::map<std::string,
                                         WorkingImagePointer>& deformedROIs, DataSet& subject ) const;
  bool                  GetNormalization() const;
  void                  SetNormalization();

  std::string           GetAtlasFilename() const;
  std::string           GetAtlasBinaryFilename() const;
  int                   GetROIAutoDilateSize() const;
  unsigned int          GetROICount() const;
  WorkingImagePointer   GetAtlasImage() const;
  ProbabilityMapList *  GetROIDataList() const;
  DataSet *      GetAtlasDataSet() const;

  BRAINSCutConfiguration::ApplyDataSetListType GetApplyDataSet() const;
  BRAINSCutConfiguration::TrainDataSetListType GetTrainDataSet() const;

  int                   GetTrainIteration() const;
  scalarType            GetANNOutputThreshold() const;
  scalarType            GetGaussianSmoothingSigma() const;

  std::string           GetSubjectToAtlasRegistrationFilename( DataSet& subject) const;
  std::string           GetAtlasToSubjectRegistrationFilename( DataSet& subject) const;

  void         SetTrainConfiguration( std::string trainParamterName );
  unsigned int GetEpochIteration() const;
  float        GetDesiredError() const;
  unsigned int GetMaximumDataSize() const;
  int          GetANNHiddenNodesNumber() const;
  float        GetActivationFunctionSlope() const;
  float        GetActivationFunctionMinMax() const;
  int          GetMaxDepth() const;
  int          GetMinSampleCount() const;
  bool         GetUseSurrogates() const;
  bool         GetCalcVarImportance() const;
  int          GetMaxTreeCount() const;


protected:
  TrainingVectorConfigurationType *   trainingVectorConfiguration;

  /* train parameters */
  TrainingParameters *TrainConfiguration;

  /** atlas data set*/
  DataSet *    atlasDataSet;
  std::string         atlasFilename;
  std::string         atlasBinaryFilename;
  WorkingImagePointer atlasImage;

  /**ProbabilityMaps*/
  ProbabilityMapList *             m_roiDataList;
  DataSet::StringVectorType        m_roiIDsInOrder;;
  unsigned int                     roiCount;

  /** registration data set */
  RegistrationConfigurationParser * registrationParser;
  std::string                       registrationImageTypeToUse;
  std::string                       registrationID;
  int                               roiAutoDilateSize;

  /** Spatial Coordinate System Images*/
  WorkingImagePointer m_rho;
  WorkingImagePointer m_phi;
  WorkingImagePointer m_theta;

  unsigned int m_gradientSize;

  /** vector file name */
  std::string trainVectorFilename;
  bool        normalization;

  /** model name **/
  std::string m_ANNModelFilename;
  std::string m_RandomForestModelFilename;
  std::string ANNTestingSSEFilename;

private:
  std::string              myConfigurationFilename;
  BRAINSCutConfiguration * myConfiguration;

  WorkingImageType GetDeformedImage( WorkingImageType image) const;


};
#endif
