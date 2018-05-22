#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fbxsdk.h>
using namespace std;

#define PIe 3.141592654


#include <fstream>
#include "bone.h"
using namespace glm;

/* Tab character ("\t") counter */
int numTabs = 0;

/**
* Print the required number of tabs.
*/
void PrintTabs(FILE *file) {
	for (int i = 0; i < numTabs; i++)
		fprintf(file,"\t");
}

/**
* Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	
	ifstream g;
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

/**
* Print an attribute.
*/
void PrintAttribute(FILE *file,FbxNodeAttribute* pAttribute) {
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	PrintTabs(file);
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	fprintf(file,"<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

/**
* Print a node, its attributes, and all its children recursively.
*/
bone *root=NULL;
int indexcount = 0;
void PrintNode(bone *actual, FbxNode* pNode, int lastlevel)
{
	

	actual->index = indexcount++;
	//actual->mat = &modelmat[actual->index];

	int level = lastlevel +1;
	cout << "bonename: " <<  pNode->GetName() << endl;
	cout << "bone level: " << level << endl;
	
	actual->name = pNode->GetName();

	FbxDouble3 bone_vector = pNode->LclTranslation.Get();

	float t1, t2, t3;
	t1 = bone_vector[0];
	t2 = bone_vector[1];
	t3= bone_vector[2];
	cout << "bone vector (x,y,z): " << t1 << ", " << t2 << ", " << t3 << endl;
	actual->pos.x = t1;
	actual->pos.y = t2;
	actual->pos.z = t3;
	// Get the node's default local transformation matrix.
	FbxAMatrix& lLocalTransform = pNode->EvaluateLocalTransform();

	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	float u0, u1, u2, u3;
	float e1, e2, e3;
	e1 = rotation[0];
	e2 = rotation[1];
	e3 = rotation[2];

	u0 = sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)* cos(e3*PIe / 180) + 1) / 2;
	u1 = (cos(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e3*PIe / 180) + sin(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u2 = (sin(e2*PIe / 180)*sin(e3*PIe / 180) - cos(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180) - sin(e1*PIe / 180)) / sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u3 = (sin(e2*PIe / 180)*cos(e1*PIe / 180) + sin(e2*PIe / 180)*cos(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	
	cout << "bone quaternion (i,j,k,re): " << u1 << ", " << u2 << ", " << u3 << ", " << u0 << endl;
	actual->q.x = u1;
	actual->q.y = u2;
	actual->q.z = u3;
	actual->q.w = u0;
	


	int child_count = pNode->GetChildCount();
	cout << "bone children count: " << child_count << endl;
	cout << "bone children names:" << endl;
	for (int j = 0; j < pNode->GetChildCount(); j++)
		{
	
		cout << "\t" << pNode->GetChild(j)->GetName() << endl;
		}

	//for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
	//	PrintAttribute(file,pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
	{
		bone *k = new bone;
		actual->kids.push_back(k);
		k->parent = actual;
		PrintNode(k,pNode->GetChild(j), level);
	}


}

void CountBones(FbxNode* pNode, int &count)
{
	count++;
	for (int j = 0; j < pNode->GetChildCount(); j++)
	{
		CountBones(pNode->GetChild(j), count);
	}

}

void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher = false);
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher = false);

void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher);
void DisplayCurveKeys(FbxAnimCurve* pCurve);
void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty);
void PrintAnimationData(all_animations *all_animation, FbxScene* lScene);
void CalcTransRotAnim(all_animations *all_animation, FbxScene* lScene, FbxNode* lNode, int animno)
{
	FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(animno);
	FbxString animStackName = currAnimStack->GetName();
	FbxString mAnimationName = animStackName.Buffer();
	FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
	long long duration = end.GetMilliSeconds();
	int keyframecount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

	const char* nodeName = lNode->GetName();
//	cout << endl << "\t" << "bone name: " << nodeName << endl << endl;

	animation_per_bone anim;
	anim.bone = nodeName;
	anim.duration = duration;
	anim.frames = keyframecount;
	anim.name = mAnimationName;


	for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
	{
	FbxTime currTime;
	currTime.SetFrame(i, FbxTime::eFrames24);
	long long time_ms=currTime.GetMilliSeconds();
	//cout << "\t" << "\t" << "frame time stamp (ms): " << time_ms << endl;
	FbxDouble3 translation = lNode->EvaluateLocalTranslation(currTime);
	float t1, t2, t3;
	t1 = translation[0];
	t2 = translation[1];
	t3 = translation[2];
//	cout << "\t" << "\t" << "translation (x,y,z): " << t1 << ", " << t2 << ", " << t3 << endl;

	FbxDouble3 rotation = lNode->EvaluateLocalRotation(currTime);
	FbxAMatrix& loctraf = lNode->EvaluateLocalTransform(currTime);

	float e1, e2, e3;
	e1 = rotation[0];
	e2 = rotation[1];
	e3 = rotation[2];

	float u0, u1, u2, u3;

	u0 = sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)* cos(e3*PIe / 180) + 1) / 2;
	u1 = (cos(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e3*PIe / 180) + sin(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u2 = (sin(e2*PIe / 180)*sin(e3*PIe / 180) - cos(e2*PIe / 180)*sin(e1*PIe / 180)*cos(e3*PIe / 180) - sin(e1*PIe / 180)) / sqrt(cos(e2*PIe / 180)*cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;
	u3 = (sin(e2*PIe / 180)*cos(e1*PIe / 180) + sin(e2*PIe / 180)*cos(e3*PIe / 180) + cos(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180)) / sqrt(cos(e2*PIe / 180)* cos(e1*PIe / 180) + cos(e2*PIe / 180)*cos(e3*PIe / 180) - sin(e2*PIe / 180)*sin(e1*PIe / 180)*sin(e3*PIe / 180) + cos(e1*PIe / 180)*cos(e3*PIe / 180) + 1) / 2;

	u3 = -u3;
	u0 = -u0;

	e1 = e1*PIe / 180;
	e2 = e2*PIe / 180;
	e3 = e3*PIe / 180;

	float q0, q1, q2, q3;
	q0 = -(cos(e1 / 2)*cos(e2 / 2)*cos(e3 / 2) + sin(e1 / 2)*sin(e2 / 2)*sin(e3 / 2));
	q1 = -(sin(e1 / 2)*cos(e2 / 2)*cos(e3 / 2) - cos(e1 / 2)*sin(e2 / 2)*sin(e3 / 2));
	q2 =-( cos(e1 / 2)*sin(e2 / 2)*cos(e3 / 2) + sin(e1 / 2)*cos(e2 / 2)*sin(e3 / 2));
	q3 =-( cos(e1 / 2)*cos(e2 / 2)*sin(e3 / 2) - sin(e1 / 2)*sin(e2 / 2)*cos(e3 / 2));
	keyframe key;
	key.timestamp_ms = time_ms;
	key.quaternion.w = q0;
	key.quaternion.x = q1;
	key.quaternion.y = q2;
	key.quaternion.z = q3;
	
	key.translation.x = t1;
	key.translation.y = t2;
	key.translation.z = t3;

	anim.keyframes.push_back(key);

//	cout << "\t" << "\t" << "quaternion (i,j,k,re): " << q1 << ", " << q2 << ", " << q3 << ", " << q0 << endl;

}
all_animation->animations.push_back(anim);

for (int k = 0; k < lNode->GetChildCount();k++)
	CalcTransRotAnim(all_animation,lScene, lNode->GetChild(k), animno);

}



//***************************************************************************************************************************************************************
void PrintAnimationData(all_animations *all_animation,FbxScene* lScene)
{
	int i;

	int count_animations = lScene->GetSrcObjectCount<FbxAnimStack>();
	
	////falls es keine animation gibt
	if (count_animations == 0)
	{
		cout << "no animations in file" << endl;
		return;
	}

	FbxNode* lNode = lScene->GetRootNode();
	for (int l = 0; l < count_animations; l++)
	{		
		FbxAnimStack* currAnimStack = lScene->GetSrcObject<FbxAnimStack>(l);
		FbxString animStackName = currAnimStack->GetName();
		FbxString mAnimationName = animStackName.Buffer();
		FbxTakeInfo* takeInfo = lScene->GetTakeInfo(animStackName);
		FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
		FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
		long long duration = end.GetMilliSeconds();
		int keyframecount = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

		cout << endl;
		cout << "animation name: " << mAnimationName << endl;
		cout << "key frame count: " << keyframecount << endl;
		cout << "animation duration (ms): " << duration << endl;
		for (int k = 0; k < lNode->GetChildCount(); k++)
			CalcTransRotAnim(all_animation,lScene, lNode->GetChild(k), l);
	}
}
//***************************************************************************************************************************************************************
void DisplayAnimation(FbxScene* pScene)
{
	int i;
	for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
	{
		FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
		FbxString lOutputString = "Animation Stack Name: ";
		lOutputString += lAnimStack->GetName();
		lOutputString += "\n\n";
		FBXSDK_printf(lOutputString);
		DisplayAnimation(lAnimStack, pScene->GetRootNode(), true);
		DisplayAnimation(lAnimStack, pScene->GetRootNode());
	}
}
void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher)
{
	int l;
	int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
	FbxString lOutputString;
	lOutputString = "Animation stack contains ";
	lOutputString += nbAnimLayers;
	lOutputString += " Animation Layer(s)\n";
	FBXSDK_printf(lOutputString);
	for (l = 0; l < nbAnimLayers; l++)
	{
		FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);
		lOutputString = "AnimLayer ";
		lOutputString += l;
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
		DisplayAnimation(lAnimLayer, pNode, isSwitcher);
	}
}
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher)
{
	int lModelCount;
	FbxString lOutputString;
	lOutputString = "     Node Name: ";
	lOutputString += pNode->GetName();
	lOutputString += "\n\n";
	FBXSDK_printf(lOutputString);
	DisplayChannels(pNode, pAnimLayer, DisplayCurveKeys, DisplayListCurveKeys, isSwitcher);
	FBXSDK_printf("\n");
	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		DisplayAnimation(pAnimLayer, pNode->GetChild(lModelCount), isSwitcher);
	}
}
void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, void(*DisplayCurve) (FbxAnimCurve* pCurve), void(*DisplayListCurve) (FbxAnimCurve* pCurve, FbxProperty* pProperty), bool isSwitcher)
{
	FbxAnimCurve* lAnimCurve = NULL;
	// Display general curves.
	if (!isSwitcher)
	{
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	
		if (lAnimCurve)
		{
			//FbxDouble3 rotation = pNode->Lcl.Get();
			//FbxDouble3 scaling = pNode->LclScaling.Get();
			FBXSDK_printf("        TX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        TZ\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        RZ\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SX\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SY\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			FBXSDK_printf("        SZ\n");
			DisplayCurve(lAnimCurve);
		}
	}
	// Display curves specific to a light or marker.
	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
	if (lNodeAttribute)
	{
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_RED);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Red\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_GREEN);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Green\n");
			DisplayCurve(lAnimCurve);
		}
		lAnimCurve = lNodeAttribute->Color.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COLOR_BLUE);
		if (lAnimCurve)
		{
			FBXSDK_printf("        Blue\n");
			DisplayCurve(lAnimCurve);
		}
		// Display curves specific to a light.
		FbxLight* light = pNode->GetLight();
		if (light)
		{
			lAnimCurve = light->Intensity.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Intensity\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = light->OuterAngle.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Outer Angle\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = light->Fog.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Fog\n");
				DisplayCurve(lAnimCurve);
			}
		}
		// Display curves specific to a camera.
		FbxCamera* camera = pNode->GetCamera();
		if (camera)
		{
			lAnimCurve = camera->FieldOfView.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->FieldOfViewX.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View X\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->FieldOfViewY.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Field of View Y\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->OpticalCenterX.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Optical Center X\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->OpticalCenterY.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Optical Center Y\n");
				DisplayCurve(lAnimCurve);
			}
			lAnimCurve = camera->Roll.GetCurve(pAnimLayer);
			if (lAnimCurve)
			{
				FBXSDK_printf("        Roll\n");
				DisplayCurve(lAnimCurve);
			}
		}
		// Display curves specific to a geometry.
		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometry* lGeometry = (FbxGeometry*)lNodeAttribute;
			int lBlendShapeDeformerCount = lGeometry->GetDeformerCount(FbxDeformer::eBlendShape);
			for (int lBlendShapeIndex = 0; lBlendShapeIndex<lBlendShapeDeformerCount; ++lBlendShapeIndex)
			{
				FbxBlendShape* lBlendShape = (FbxBlendShape*)lGeometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);
				int lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
				for (int lChannelIndex = 0; lChannelIndex<lBlendShapeChannelCount; ++lChannelIndex)
				{
					FbxBlendShapeChannel* lChannel = lBlendShape->GetBlendShapeChannel(lChannelIndex);
					const char* lChannelName = lChannel->GetName();
					lAnimCurve = lGeometry->GetShapeChannel(lBlendShapeIndex, lChannelIndex, pAnimLayer, true);
					if (lAnimCurve)
					{
						FBXSDK_printf("        Shape %s\n", lChannelName);
						DisplayCurve(lAnimCurve);
					}
				}
			}
		}
	}
	// Display curves specific to properties
	FbxProperty lProperty = pNode->GetFirstProperty();
	while (lProperty.IsValid())
	{
		if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
		{
			FbxString lFbxFCurveNodeName = lProperty.GetName();
			FbxAnimCurveNode* lCurveNode = lProperty.GetCurveNode(pAnimLayer);
			if (!lCurveNode) {
				lProperty = pNode->GetNextProperty(lProperty);
				continue;
			}
			FbxDataType lDataType = lProperty.GetPropertyDataType();
			if (lDataType.GetType() == eFbxBool || lDataType.GetType() == eFbxDouble || lDataType.GetType() == eFbxFloat || lDataType.GetType() == eFbxInt)
			{
				FbxString lMessage;
				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				};
				//DisplayString(lMessage.Buffer());
				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
						DisplayCurve(lAnimCurve);
				}
			}
			else if (lDataType.GetType() == eFbxDouble3 || lDataType.GetType() == eFbxDouble4 || lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT))
			{
				char* lComponentName1 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_RED : (char*)"X";
				char* lComponentName2 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_GREEN : (char*)"Y";
				char* lComponentName3 = (lDataType.Is(FbxColor3DT) || lDataType.Is(FbxColor4DT)) ? (char*)FBXSDK_CURVENODE_COLOR_BLUE : (char*)"Z";
				FbxString      lMessage;

				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				}
				//DisplayString(lMessage.Buffer());
				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
					{
						//DisplayString("        Component ", lComponentName1);
						DisplayCurve(lAnimCurve);
					}
				}
				for (int c = 0; c < lCurveNode->GetCurveCount(1U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(1U, c);
					if (lAnimCurve)
					{
						//DisplayString("        Component ", lComponentName2);
						DisplayCurve(lAnimCurve);
					}
				}
				for (int c = 0; c < lCurveNode->GetCurveCount(2U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(2U, c);
					if (lAnimCurve)
					{
						//DisplayString("        Component ", lComponentName3);
						DisplayCurve(lAnimCurve);
					}
				}
			}
			else if (lDataType.GetType() == eFbxEnum)
			{
				FbxString lMessage;
				lMessage = "        Property ";
				lMessage += lProperty.GetName();
				if (lProperty.GetLabel().GetLen() > 0)
				{
					lMessage += " (Label: ";
					lMessage += lProperty.GetLabel();
					lMessage += ")";
				};
				//DisplayString(lMessage.Buffer());
				for (int c = 0; c < lCurveNode->GetCurveCount(0U); c++)
				{
					lAnimCurve = lCurveNode->GetCurve(0U, c);
					if (lAnimCurve)
						DisplayListCurve(lAnimCurve, &lProperty);
				}
			}
		}
		lProperty = pNode->GetNextProperty(lProperty);
	} // while
}
static int InterpolationFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
	return 0;
}
static int ConstantmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
	return 0;
}
static int TangentmodeFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
	if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
	if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
	return 0;
}
static int TangentweightFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
	return 0;
}
static int TangentVelocityFlagToIndex(int flags)
{
	if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
	return 0;
}
void DisplayCurveKeys(FbxAnimCurve* pCurve)
{
	static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
	static const char* constantMode[] = { "?", "Standard", "Next" };
	static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
	static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };
	FbxTime   lKeyTime;
	float   lKeyValue;
	char    lTimeString[256];
	FbxString lOutputString;
	int     lCount;
	int lKeyCount = pCurve->KeyGetCount();
	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);
		
		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " [ ";
		lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
		if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			lOutputString += " | ";
			lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
		}
		else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			lOutputString += " | ";
			lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
			lOutputString += " | ";
			lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
		}
		lOutputString += " ]";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}
void DisplayListCurveKeys(FbxAnimCurve* pCurve, FbxProperty* pProperty)
{
	FbxTime   lKeyTime;
	int     lKeyValue;
	char    lTimeString[256];
	FbxString lListValue;
	FbxString lOutputString;
	int     lCount;
	int lKeyCount = pCurve->KeyGetCount();
	for (lCount = 0; lCount < lKeyCount; lCount++)
	{
		lKeyValue = static_cast<int>(pCurve->KeyGetValue(lCount));
		lKeyTime = pCurve->KeyGetTime(lCount);
		lOutputString = "            Key Time: ";
		lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
		lOutputString += ".... Key Value: ";
		lOutputString += lKeyValue;
		lOutputString += " (";
		lOutputString += pProperty->GetEnumValue(lKeyValue);
		lOutputString += ")";
		lOutputString += "\n";
		FBXSDK_printf(lOutputString);
	}
}










/**
* Main function - loads the hard-coded fbx file,
* and prints its contents in an xml format to stdout.
*/

int readtobone(string file, all_animations *all_animation,bone **proot)
{

	//ifstream fileHandle("fgdfg");
	string name_of_file;
	cout << endl << "filename:" << endl;
	name_of_file = file;
	const char* lFilename = name_of_file.c_str();
	FILE *checkfile=fopen(lFilename,"rb");
	if (!checkfile)
		{
		cout << endl << "file not found!" << endl;
		return 0;
		}
	else
		cout << endl << "file exists!" << endl;
	fclose(checkfile);
	

	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();

	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		FbxString error = lImporter->GetStatus().GetErrorString();
		exit(-1);
	}

	// Create a new scene so that it can be populated by the imported file.
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");

	// Import the contents of the file into the scene.
	lImporter->Import(lScene);

	// The file is imported; so get rid of the importer.
	lImporter->Destroy();

	string create_file_path;
	
	
	///////////////////
	///	Model (Skeleton)
	/////////////////
	// Print the nodes of the scene and their attributes recursively.
	// Note that we are not printing the root node because it should
	// not contain any attributes.
	FbxNode* lRootNode = lScene->GetRootNode();
	int count_bones=0;
	int child_count = lRootNode->GetChildCount();
	for (int i = 0; i < child_count; i++)//nur einen knochen machen
		CountBones(lRootNode->GetChild(i),count_bones);

	cout << endl;
	cout << "Skeleton" << endl;
	cout << endl;
	cout << "count bones: " << count_bones << endl;
	
	
		bone *root = new bone;
		*proot = root;

	if (lRootNode) 	
	{
		int anz = lRootNode->GetChildCount();
		for (int i = 0; i < lRootNode->GetChildCount(); i++)//nur einen knochen machen
			{
			PrintNode(root,lRootNode->GetChild(i), -1);
			}			
	}

	cout << "----------------------------------------------------------------------------------------------------" << endl;
	///////////////////
	///	Animation Data 
	/////////////////
	//extract animation stacks
	/*int numStacks = lScene->GetSrcObjectCount(FBX_TYPE(FbxAnimStack));*/

	//cout << endl;
	//cout << "Animation" << endl;	
	PrintAnimationData(all_animation,lScene);

	/////////////////////
	/////	End
	///////////////////
	//// Destroy the SDK manager and all the other objects it was handling.
	lSdkManager->Destroy();
	//system("pause");
	return 0;
}



