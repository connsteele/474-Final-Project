#pragma once
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class keyframe
	{
	public:
		quat quaternion;
		vec3 translation;
		long long timestamp_ms;
	};
class animation_per_bone
	{
	public:
		string name;
		long long duration;
		int frames;
		string bone;
		vector<keyframe> keyframes;
	};
class all_animations
	{
	public:
		vector<animation_per_bone> animations;

	};


//**************************************************

class bone
{
public:
	vector<animation_per_bone*> animation;	//all the animations of the bone
	string name;
	vec3 pos;
	quat q;
	bone *parent = NULL;
	vector<bone*> kids;			
	unsigned int index;			//a unique number for each bone, at the same time index of the animatiom matrix array
	mat4 *mat = NULL;			//address of one lement from the animation matrix array
	// searches for the animation and sets the animation matrix element to the recent matrix gained from the keyframe
	void play_animation(int keyframenumber, string animationname)
		{
		for (int i = 0; i < animation.size(); i++)
			if (animation[i]->name == animationname)
				{
				if (animation[i]->keyframes.size() > keyframenumber)
					{
					quat q = animation[i]->keyframes[keyframenumber].quaternion;
					vec3 tr = animation[i]->keyframes[keyframenumber].translation;
					mat4 M = mat4(q);
					mat4 T = translate(mat4(1), tr);
					M = T * M;
					if (mat)
						{
						mat4 parentmat = mat4(1);
						if (parent)
							parentmat = *parent->mat;
						*mat = parentmat * M;
						}
					}
				else
					*mat = mat4(1);
				}
		for (int i = 0; i < kids.size(); i++)
			kids[i]->play_animation(keyframenumber,animationname);
		}


	void myplayanimation(float keyframenumber, int animationnum, int animation2num, float f) {

		if (animation[animation2num]->keyframes.size() > keyframenumber +1)
		{
			float ratio = 1. * animation[animationnum]->keyframes.size() / animation[animation2num]->keyframes.size();


			float fframe = (float)keyframenumber;
			int framea = fframe;
			int frameb = fframe + 1;
			float t = fframe - (int)fframe;

			quat qa = animation[animationnum]->keyframes[framea * ratio].quaternion;
			quat qb = animation[animationnum]->keyframes[frameb * ratio].quaternion;
			quat qr = slerp(qa, qb, t);

			vec3 ta = animation[animationnum]->keyframes[framea*ratio].translation;
			vec3 tb = animation[animationnum]->keyframes[frameb*ratio].translation;
			vec3 tr = mix(ta, tb, t);

			quat qc = animation[animation2num]->keyframes[framea].quaternion;
			quat qd = animation[animation2num]->keyframes[frameb].quaternion;
			quat qe = slerp(qc, qd, t);

			vec3 tc = animation[animation2num]->keyframes[framea].translation;
			vec3 td = animation[animation2num]->keyframes[frameb].translation;
			vec3 te = mix(tc, td, t);

			quat qf = slerp(qr, qe, f);
			vec3 tf = mix(tr, te, f);


			mat4 M = mat4(qf);
			mat4 T = translate(mat4(1), tf);
			M = T * M;
			if (mat)
			{
				mat4 parentmat = mat4(1);
				if (parent)
					parentmat = *parent->mat;
				*mat = parentmat * M;
			}

			else
				*mat = mat4(1);

			for (int i = 0; i < kids.size(); i++)
				kids[i]->myplayanimation(keyframenumber, animationnum, animation2num, f);

		}

	}

	//writes into the segment positions and into the animation index VBO
	void write_to_VBOs(vec3 origin, vector<vec3> &vpos, vector<unsigned int> &imat)
		{
		vpos.push_back(origin);
		vec3 endp = origin + pos;
		vpos.push_back(endp);

		if(parent)
			imat.push_back(parent->index);
		else
			imat.push_back(index);
		imat.push_back(index);

		for (int i = 0; i < kids.size(); i++)
			kids[i]->write_to_VBOs(endp, vpos, imat);
		}
	//searches for the correct animations as well as sets the correct element from the animation matrix array
	void set_animations(all_animations *all_anim,mat4 *matrices,int &animsize)
		{
		for (int ii = 0; ii < all_anim->animations.size(); ii++)
			if (all_anim->animations[ii].bone == name)
				animation.push_back(&all_anim->animations[ii]);

		mat = &matrices[index];
		animsize++;

		for (int i = 0; i < kids.size(); i++)
			kids[i]->set_animations(all_anim, matrices, animsize);
		}

	int getKeyFrameCount(std::string animationName) {
		for (auto anim : animation) {
			if (anim->name == animationName) {
				return anim->frames;
			}
		}

		return 1;
	}

	long long getDuration(std::string animationName) {
		for (auto anim : animation) {
			if (anim->name == animationName) {
				return anim->duration;
			}
		}

		return 0;
	}

};
int readtobone(string file,all_animations *all_animation, bone **proot);