#include "BRAINSCutDataHandler.h"
#include "XMLConfigurationFileParser.h"
#include "GenericTransformImage.h"

/** constructors */
BRAINSCutDataHandler
::BRAINSCutDataHandler( const std::string modelConfigurationFilename )
{
  try
    {
    SetNetConfigurationFilename( modelConfigurationFilename );
    SetNetConfiguration();
    }
  catch( BRAINSCutExceptionStringHandler& e)
    {
      std::cout<<e.Error()<<std::endl;
      exit(EXIT_FAILURE);
    }
}

void
BRAINSCutDataHandler
::SetNetConfiguration()
{
  try
    {
    //
    // read in xml images
    //
    std::list<ElementContainer *> elementList;

    elementList.push_front( myConfiguration );

    XMLConfigurationFileParser BRAINSCutXMLConfigurationFileParser =
      XMLConfigurationFileParser( myConfigurationFilename );
    BRAINSCutXMLConfigurationFileParser.SetUserData( &elementList );
    BRAINSCutXMLConfigurationFileParser.Parse();

    myConfiguration = BRAINSCutXMLConfigurationFileParser.GetConfiguration();
    }
  catch( BRAINSCutExceptionStringHandler& e)
    {
      std::cout<<e.Error()<<std::endl;
      exit(EXIT_FAILURE);
    }
}

void
BRAINSCutDataHandler
::SetAtlasDataSet()
{
  atlasDataSet = myConfiguration->GetAtlasDataSet();
  std::cout<<"registrationImageTypeToUse :: "<<registrationImageTypeToUse <<std::endl;

  if( registrationImageTypeToUse.empty() )
  {
    std::cout<<"registrationImageTypeToUse is empty."<<std::endl;
    exit(EXIT_FAILURE);
  }
  atlasFilename=atlasDataSet->GetImageFilenameByType( registrationImageTypeToUse) ;
  atlasBinaryFilename=atlasDataSet->GetMaskFilenameByType( "RegistrationROI" );
  std::cout<<atlasBinaryFilename<<std::endl;
}

void
BRAINSCutDataHandler
::SetAtlasImage()
{
  atlasImage = ReadImageByFilename( atlasFilename );
}

void
BRAINSCutDataHandler
::SetRhoPhiTheta()
{
  this->m_rho = ReadImageByFilename( atlasDataSet->GetSpatialLocationFilenameByType("rho") );
  this->m_phi = ReadImageByFilename( atlasDataSet->GetSpatialLocationFilenameByType("phi") );
  this->m_theta = ReadImageByFilename( atlasDataSet->GetSpatialLocationFilenameByType("theta") );

}

void
BRAINSCutDataHandler
::SetNetConfigurationFilename(const std::string filename)
{
  myConfigurationFilename = filename;
}

std::string
BRAINSCutDataHandler
::GetNetConfigurationFilename() const
{
  return myConfigurationFilename;
}

int
BRAINSCutDataHandler
::GetTrainIteration() const
{
  const int trainIteration = myConfiguration->Get<TrainingParameters>("ANNParameters")
                            ->GetAttribute<IntValue>("Iterations");
  return trainIteration;
}

DataSet::StringVectorType
BRAINSCutDataHandler
::GetROIIDsInOrder() const
{
  return this->m_roiIDsInOrder;
}

void
BRAINSCutDataHandler
::SetRegionsOfInterest()
{
  this->m_roiDataList = myConfiguration->Get<ProbabilityMapList>("ProbabilityMapList");
  this->m_roiIDsInOrder = this->m_roiDataList->CollectAttValues<ProbabilityMapParser>("StructureID");

  std::sort( this->m_roiIDsInOrder.begin(), this->m_roiIDsInOrder.end() ); // get l_caudate, l_globus, .. , r_caudate, r_globus..
  roiCount = this->m_roiDataList->size();
}

/** registration related */
void
BRAINSCutDataHandler
::SetRegistrationParameters()
{

  registrationParser =
    myConfiguration->Get<RegistrationConfigurationParser>("RegistrationConfiguration");

  SetRegistrationImageTypeToUse (
      std::string( registrationParser->GetAttribute<StringValue>( "ImageTypeToUse") ) );

  registrationID = std::string(
    registrationParser->GetAttribute<StringValue>("ID") );

  roiAutoDilateSize = registrationParser->GetAttribute<IntValue>("BRAINSROIAutoDilateSize") ;
}

void
BRAINSCutDataHandler
::SetRegistrationImageTypeToUse( std::string type )
{
  registrationImageTypeToUse=type;
}

std::string
BRAINSCutDataHandler
::GetRegistrationImageTypeToUse() const
{
  if( registrationImageTypeToUse.empty() )
  {
    std::cout<<"registrationImageTypeToUse is empty."<<std::endl;
    exit(EXIT_FAILURE);
  }
  return registrationImageTypeToUse;
}

std::string
BRAINSCutDataHandler
::GetRegistrationID() const
{
  if( registrationID.empty() )
  {
    std::cout<<"The registrationID is empty"<<std::endl;
    exit(EXIT_FAILURE);
  }
  return registrationID;
}

std::string
BRAINSCutDataHandler
::GetSubjectToAtlasRegistrationFilename( DataSet& subject) const
{
  std::string filename = subject.GetRegistrationWithID( registrationID )
    ->GetAttribute<StringValue>("SubjToAtlasRegistrationFilename");
  return filename;
}

std::string
BRAINSCutDataHandler
::GetAtlasToSubjectRegistrationFilename( DataSet& subject) const
{
  std::string filename = subject.GetRegistrationWithID( registrationID )
    ->GetAttribute<StringValue>("AtlasToSubjRegistrationFilename");
  return filename;
}

void
BRAINSCutDataHandler
::GetDeformedSpatialLocationImages( std::map<std::string, WorkingImagePointer>& warpedSpatialLocationImages,
                                    DataSet& subject) const
{
  std::string atlasSubjectRegistrationFilename = GetAtlasToSubjectRegistrationFilename( subject );

  DisplacementFieldType::Pointer deformation = GetDeformationField( atlasSubjectRegistrationFilename );
  GenericTransformType::Pointer genericTransform = GetGenericTransform( atlasSubjectRegistrationFilename );

  std::string subjectFilenameToUse = subject.GetImageFilenameByType(registrationImageTypeToUse);
  if( !itksys::SystemTools::FileExists( subjectFilenameToUse.c_str(), false ) )
    {
    std::cout << " Subject image file of "
              << subjectFilenameToUse
              << ", type of " << registrationImageTypeToUse
              << ", does not exist. "
              << std::endl;
    exit(EXIT_FAILURE);
    }

  WorkingImagePointer referenceImage =
    ReadImageByFilename( subject.GetImageFilenameByType(registrationImageTypeToUse) );

  const std::string transoformationPixelType = "float";

  warpedSpatialLocationImages.insert( std::pair<std::string, WorkingImagePointer>
                                        ("rho", GenericTransformImage<WorkingImageType,
                                                                      WorkingImageType,
                                                                      DisplacementFieldType>
                                          ( this->m_rho, referenceImage, deformation, genericTransform,
                                          0.0, "Linear", transoformationPixelType == "binary") ) );
  warpedSpatialLocationImages.insert( std::pair<std::string, WorkingImagePointer>
                                        ("phi", GenericTransformImage<WorkingImageType,
                                                                      WorkingImageType,
                                                                      DisplacementFieldType>
                                          ( this->m_phi, referenceImage, deformation, genericTransform,
                                          0.0, "Linear", transoformationPixelType == "binary") ) );
  warpedSpatialLocationImages.insert( std::pair<std::string, WorkingImagePointer>
                                        ("theta", GenericTransformImage<WorkingImageType,
                                                                        WorkingImageType,
                                                                        DisplacementFieldType>
                                          ( this->m_theta, referenceImage, deformation, genericTransform,
                                          0.0, "Linear", transoformationPixelType == "binary") ) );
}

void
BRAINSCutDataHandler
::GetImagesOfSubjectInOrder( WorkingImageVectorType& subjectImageList, DataSet& subject) const
{
  DataSet::StringVectorType imageListFromAtlas = atlasDataSet->GetImageTypes(); // T1, T2, SG, ...
  std::sort( imageListFromAtlas.begin(), imageListFromAtlas.end() );            // SG, T1, T2, ... ascending order
  for( DataSet::StringVectorType::iterator imgTyIt = imageListFromAtlas.begin();
       imgTyIt != imageListFromAtlas.end();
       ++imgTyIt ) // imgTyIt = image type iterator
    {
    std::cout<< *imgTyIt <<std::endl;
    WorkingImagePointer currentTypeImage = ReadImageByFilename( subject.GetImageFilenameByType( *imgTyIt ) );
    subjectImageList.push_back( currentTypeImage );
    }
}

void
BRAINSCutDataHandler
::GetDeformedROIs( std::map<std::string, WorkingImagePointer>& warpedROIs,
                   DataSet& subject) const
{
  const std::string atlasSubjectRegistrationFilename = GetAtlasToSubjectRegistrationFilename( subject );

  /** Get the transformation file
   * Note that only one of transformation type will be used. Either deformation or transformation
   * That determined based on the file name at the GetDeformationField
   */
  DisplacementFieldType::Pointer deformation = GetDeformationField( atlasSubjectRegistrationFilename );
  GenericTransformType::Pointer genericTransform = GetGenericTransform( atlasSubjectRegistrationFilename );

  WorkingImagePointer referenceImage =
    ReadImageByFilename( subject.GetImageFilenameByType(registrationImageTypeToUse) );

  const std::string transformationPixelType = "float";
  for( DataSet::StringVectorType::const_iterator roiTyIt = this->m_roiIDsInOrder.begin();
       roiTyIt != this->m_roiIDsInOrder.end();
       ++roiTyIt )
    {
    const std::string roiFilename = this->m_roiDataList->GetMatching<ProbabilityMapParser>( "StructureID", (*roiTyIt).c_str() )
      ->GetAttribute<StringValue>("Filename");
    WorkingImagePointer currentROI = ReadImageByFilename( roiFilename );

    warpedROIs.insert( std::pair<std::string, WorkingImagePointer>(
                         (*roiTyIt), GenericTransformImage<WorkingImageType,
                                                           WorkingImageType,
                                                           DisplacementFieldType>
                           ( currentROI, referenceImage, deformation,
                           genericTransform, 0.0, "Linear",
                           transformationPixelType == "binary") ) );
    }
}

void
BRAINSCutDataHandler
::SetTrainingVectorConfiguration()
{
  std::cout<<__LINE__<<"::"<<__FILE__<<std::endl;
  trainingVectorConfiguration = myConfiguration->Get<TrainingVectorConfigurationType>("TrainingVectorConfiguration");
  std::cout<<__LINE__<<"::"<<__FILE__<<std::endl;
}

void
BRAINSCutDataHandler
::SetGradientSize()
{
  this->m_gradientSize = trainingVectorConfiguration->GetAttribute<IntValue>("GradientProfileSize");
}

unsigned int
BRAINSCutDataHandler
::GetGradientSize() const
{
  return this->m_gradientSize ;
}

void
BRAINSCutDataHandler
::SetNormalization()
{
  std::string normalizationString;
  try
    {
    normalizationString = trainingVectorConfiguration->GetAttribute<StringValue>("Normalization");
    }catch( BRAINSCutExceptionStringHandler& e)
    {
      std::cout<<e.Error()<<std::endl;
      exit(EXIT_FAILURE);
    }

  if( normalizationString == "true" )
    {
    normalization = true;
    }
  else
    {
    normalization =  false;
    }
  std::cout<<"Get Normalization from XML file --> "<<normalization<<std::endl;
}
bool
BRAINSCutDataHandler
::GetNormalization() const
{
  return normalization;
}

/** model file name */
std::string
BRAINSCutDataHandler
::GetModelBaseName( ) const
{
  std::string basename;
  try
    {
    basename = trainingVectorConfiguration->GetAttribute<StringValue>("TrainingModelFilename");
    }
  catch( ... )
    {
    std::cout<<"Fail to get the ann model file name:: "<< basename<<std::endl;
    throw BRAINSCutExceptionStringHandler("Fail to get the ann model file name");
    exit(EXIT_FAILURE);
    }
  return  basename;
}

std::string
BRAINSCutDataHandler
::GetANNModelFilename( ) const
{
  return this->m_ANNModelFilename;
}

#if 0
std::string
BRAINSCutDataHandler
::GetANNModelFilenameAtIteration( const int iteration) const
{
  return this->m_ANNModelFilename;
}
#endif

void
BRAINSCutDataHandler
::SetANNModelFilenameAtIteration( const int iteration)
{
  this->m_ANNModelFilename = GetModelBaseName();

  char temp[10];
  sprintf( temp, "%09d", iteration );
  this->m_ANNModelFilename += temp;
}

std::string
BRAINSCutDataHandler
::GetRFModelFilename( const int depth,
                      const int NTrees) const
{
  std::cout<<__LINE__<<"::"<<__FILE__<<std::endl;
  std::string basename = GetModelBaseName();

  std::cout<<__LINE__<<"::"<<__FILE__<<std::endl;
  char tempDepth[5];
  sprintf( tempDepth, "%04u", depth );
  std::cout<<__LINE__<<"::"<<__FILE__<<std::endl;

  char tempNTrees[5];
  sprintf( tempNTrees, "%04u", NTrees );
  std::cout<<__LINE__<<"::"<<__FILE__<<std::endl;

  std::string filename = basename + "D"+tempDepth+"NT"+tempNTrees;
  if( !itksys::SystemTools::FileExists( filename.c_str(), false ) )
    {
    filename+=".gz";
    if( !itksys::SystemTools::FileExists( filename.c_str(), false ) )
      {
      filename="ERROR_NO_FILENAME_FOUND";
      }
    }

  std::cout<<__LINE__<<"::"<<__FILE__<< "  RFModelFilename: "<< filename << std::endl;
  return filename;
}

std::string
BRAINSCutDataHandler
::GetAtlasFilename() const
{
  return atlasFilename;
}

std::string
BRAINSCutDataHandler
::GetAtlasBinaryFilename() const
{
  return atlasBinaryFilename;
}

int
BRAINSCutDataHandler
::GetROIAutoDilateSize() const
{
  return roiAutoDilateSize;
}

unsigned int
BRAINSCutDataHandler
::GetROICount() const
{
  return roiCount;
}

WorkingImagePointer
BRAINSCutDataHandler
::GetAtlasImage() const
{
  return atlasImage;
}
ProbabilityMapList *
BRAINSCutDataHandler
::GetROIDataList() const
{
  return this->m_roiDataList;
}

DataSet *
BRAINSCutDataHandler
::GetAtlasDataSet() const
{
  return atlasDataSet;
}

scalarType
BRAINSCutDataHandler
::GetANNOutputThreshold() const
{
  const scalarType annOutputThreshold =
    myConfiguration->Get<ApplyModelType>("ApplyModel")
              ->GetAttribute<FloatValue>("MaskThresh");
  if( annOutputThreshold < 0.0F )
    {
    std::string msg = " ANNOutput Threshold cannot be less than zero. \n";
    throw BRAINSCutExceptionStringHandler( msg );
    }
  return annOutputThreshold;
}

//
// Apply related
//
void
BRAINSCutDataHandler
::SetRandomForestModelFilename(std::string name)
{
  this->m_RandomForestModelFilename = name;
}

void
BRAINSCutDataHandler
::SetRandomForestModelFilename(int depth, int nTree)
{
  if( depth < 0 && nTree <0 )
    {
    nTree= myConfiguration->Get<TrainingParameters>("RandomForestParameters")
                            ->GetAttribute<IntValue>("MaxDepth");
    depth= myConfiguration->Get<TrainingParameters>("RandomForestParameters")
                            ->GetAttribute<IntValue>("MaxTreeCount");
    }
  this->m_RandomForestModelFilename =  GetRFModelFilename( depth, nTree );
}

std::string
BRAINSCutDataHandler
::GetRandomForestModelFilename() const
{
  return this->m_RandomForestModelFilename;
}

void
BRAINSCutDataHandler
::SetANNTestingSSEFilename()
{
  ANNTestingSSEFilename = GetModelBaseName();
  ANNTestingSSEFilename += "ValidationSetSSE.txt";
}

std::string
BRAINSCutDataHandler
::GetANNTestingSSEFilename() const
{
  return ANNTestingSSEFilename;
}

void
BRAINSCutDataHandler
::SetTrainVectorFilename()
{
  trainVectorFilename =
    trainingVectorConfiguration->GetAttribute<StringValue>("TrainingVectorFilename");
  trainVectorFilename += "ANN"; // TODO
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << "vector file at " << trainVectorFilename << std::endl;
  std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

std::string
BRAINSCutDataHandler
::GetTrainVectorFilename() const
{
  if( trainVectorFilename.empty() )
  {
    std::string msg = "The train vector file name is empty.\n";
    throw BRAINSCutExceptionStringHandler( msg );
  }
  return trainVectorFilename;
}

BRAINSCutConfiguration::TrainDataSetListType
BRAINSCutDataHandler
::GetTrainDataSet() const
{
  BRAINSCutConfiguration::TrainDataSetListType trainDataSetList;
  try
    {
      trainDataSetList = myConfiguration->GetTrainDataSets();
    }
  catch( BRAINSCutExceptionStringHandler& e )
    {
    std::cout << e.Error() << std::endl;
    exit(EXIT_SUCCESS);
    }
  return trainDataSetList;
}

BRAINSCutConfiguration::ApplyDataSetListType
BRAINSCutDataHandler
::GetApplyDataSet() const
{
  BRAINSCutConfiguration::ApplyDataSetListType applyDataSetList;
  try
    {
      applyDataSetList = myConfiguration->GetApplyDataSets();
    }
  catch( BRAINSCutExceptionStringHandler& e )
    {
    std::cout << e.Error() << std::endl;
    exit(EXIT_SUCCESS);
    }
  return applyDataSetList;
}

scalarType
BRAINSCutDataHandler
::GetGaussianSmoothingSigma() const
{
  const scalarType gaussianSmoothingSigma=
    myConfiguration->Get<ApplyModelType>("ApplyModel")
              ->GetAttribute<FloatValue>("GaussianSmoothingSigma");
  return gaussianSmoothingSigma;
}

void
BRAINSCutDataHandler
::SetTrainConfiguration( std::string trainParameterName )
{
  TrainConfiguration = myConfiguration->Get<TrainingParameters>( trainParameterName.c_str() );
}

unsigned int
BRAINSCutDataHandler
::GetEpochIteration() const
{
  const unsigned int trainEpochIteration = TrainConfiguration->GetAttribute<IntValue>("EpochIterations");
  return trainEpochIteration;
}

float
BRAINSCutDataHandler
::GetDesiredError() const
{
  const float trainDesiredError = TrainConfiguration->GetAttribute<FloatValue>("DesiredError");
  return trainDesiredError;
}

unsigned int
BRAINSCutDataHandler
::GetMaximumDataSize() const
{
  const unsigned int trainMaximumDataSize = TrainConfiguration->GetAttribute<IntValue>("MaximumVectorsPerEpoch");
  return trainMaximumDataSize;
}

int
BRAINSCutDataHandler
::GetANNHiddenNodesNumber() const
{
  const int ANNHiddenNodesNumber = TrainConfiguration->GetAttribute<IntValue>("NumberOfHiddenNodes");
  return ANNHiddenNodesNumber;
}

float
BRAINSCutDataHandler
::GetActivationFunctionSlope() const
{
  const float activationSlope = TrainConfiguration->GetAttribute<FloatValue>("ActivationSlope");
  return activationSlope;
}

float
BRAINSCutDataHandler
::GetActivationFunctionMinMax() const
{
  const float activationMinMax = TrainConfiguration->GetAttribute<FloatValue>("ActivationMinMax");
  return activationMinMax;
}

/** Get Random Tree */
int
BRAINSCutDataHandler
::GetMaxDepth() const
{
  const int trainMaxDepth= TrainConfiguration->GetAttribute<IntValue>("MaxDepth");
  return trainMaxDepth;
}

int
BRAINSCutDataHandler
::GetMinSampleCount() const
{
  const int trainMinSampleCount= TrainConfiguration->GetAttribute<IntValue>("MinSampleCount");
  return trainMinSampleCount;
}

bool
BRAINSCutDataHandler
::GetUseSurrogates() const
{
  const bool trainUseSurrogates= TrainConfiguration->GetAttribute<BooleanValue>("UseSurrogates");
  return trainUseSurrogates;
}

bool
BRAINSCutDataHandler
::GetCalcVarImportance() const
{
  const bool trainCalcVarImportance= TrainConfiguration->GetAttribute<BooleanValue>("CalcVarImportance");
  return trainCalcVarImportance;
}

int
BRAINSCutDataHandler
::GetMaxTreeCount() const
{
  const int trainMaxTreeCount= TrainConfiguration->GetAttribute<IntValue>("MaxTreeCount");
  return trainMaxTreeCount;
}

