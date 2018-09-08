#include "stdafx.h"
#include "ObjectWindow.h"
#include "wiSceneSystem.h"


using namespace wiECS;
using namespace wiSceneSystem;


ObjectWindow::ObjectWindow(wiGUI* gui) : GUI(gui)
{
	assert(GUI && "Invalid GUI!");


	float screenW = (float)wiRenderer::GetDevice()->GetScreenWidth();
	float screenH = (float)wiRenderer::GetDevice()->GetScreenHeight();

	objectWindow = new wiWindow(GUI, "Object Window");
	objectWindow->SetSize(XMFLOAT2(600, 400));
	objectWindow->SetEnabled(false);
	GUI->AddWidget(objectWindow);

	float x = 450;
	float y = 0;

	renderableCheckBox = new wiCheckBox("Renderable: ");
	renderableCheckBox->SetTooltip("Set object to be participating in rendering.");
	renderableCheckBox->SetPos(XMFLOAT2(x, y += 30));
	renderableCheckBox->SetCheck(true);
	renderableCheckBox->OnClick([&](wiEventArgs args) {
		ObjectComponent* object = wiRenderer::GetScene().objects.GetComponent(entity);
		if (object != nullptr)
		{
			object->renderable = args.bValue;
		}
	});
	objectWindow->AddWidget(renderableCheckBox);

	ditherSlider = new wiSlider(0, 1, 0, 1000, "Dither: ");
	ditherSlider->SetTooltip("Adjust dithered transparency of the object. This disables some optimizations so performance can be affected.");
	ditherSlider->SetSize(XMFLOAT2(100, 30));
	ditherSlider->SetPos(XMFLOAT2(x, y += 30));
	ditherSlider->OnSlide([&](wiEventArgs args) {
		ObjectComponent* object = wiRenderer::GetScene().objects.GetComponent(entity);
		if (object != nullptr)
		{
			object->color.w = 1 - args.fValue;
		}
	});
	objectWindow->AddWidget(ditherSlider);

	cascadeMaskSlider = new wiSlider(0, 3, 0, 3, "Cascade Mask: ");
	cascadeMaskSlider->SetTooltip("How many shadow cascades to skip when rendering this object into shadow maps? (0: skip none, it will be in all cascades, 1: skip first (biggest cascade), ...etc...");
	cascadeMaskSlider->SetSize(XMFLOAT2(100, 30));
	cascadeMaskSlider->SetPos(XMFLOAT2(x, y += 30));
	cascadeMaskSlider->OnSlide([&](wiEventArgs args) {
		ObjectComponent* object = wiRenderer::GetScene().objects.GetComponent(entity);
		if (object != nullptr)
		{
			object->cascadeMask = args.iValue;
		}
	});
	objectWindow->AddWidget(cascadeMaskSlider);


	colorPicker = new wiColorPicker(GUI, "Object Color");
	colorPicker->SetPos(XMFLOAT2(10, 30));
	colorPicker->RemoveWidgets();
	colorPicker->SetVisible(true);
	colorPicker->SetEnabled(true);
	colorPicker->OnColorChanged([&](wiEventArgs args) {
		ObjectComponent* object = wiRenderer::GetScene().objects.GetComponent(entity);
		if (object != nullptr)
		{
			object->color = XMFLOAT4(powf(args.color.x, 1.f / 2.2f), powf(args.color.y, 1.f / 2.2f), powf(args.color.z, 1.f / 2.2f), object->color.w);
		}
	});
	objectWindow->AddWidget(colorPicker);

	y += 60;

	physicsLabel = new wiLabel("PHYSICSLABEL");
	physicsLabel->SetText("PHYSICS SETTINGS");
	physicsLabel->SetPos(XMFLOAT2(x - 30, y += 30));
	physicsLabel->SetSize(XMFLOAT2(150, 20));
	objectWindow->AddWidget(physicsLabel);

	simulationTypeComboBox = new wiComboBox("Simulation Type:");
	simulationTypeComboBox->SetSize(XMFLOAT2(100, 20));
	simulationTypeComboBox->SetPos(XMFLOAT2(x, y += 30));
	simulationTypeComboBox->AddItem("None");
	simulationTypeComboBox->AddItem("Rigid Body");
	//simulationTypeComboBox->AddItem("Soft Body");
	simulationTypeComboBox->OnSelect([&](wiEventArgs args) {
		//PhysicsComponent* physicscomponent = wiRenderer::GetScene().physicscomponents.GetComponent(entity);
		//if (physicscomponent != nullptr)
		//{
		//	wiRenderer::physicsEngine->removeObject(*physicscomponent);

		//	switch (args.iValue)
		//	{
		//	case 0:
		//		physicscomponent->rigidBody = false;
		//		//if (object->mesh != nullptr)
		//		//{
		//		//	object->mesh->softBody = false;
		//		//}

		//		kinematicCheckBox->SetEnabled(false);
		//		physicsTypeComboBox->SetEnabled(false);
		//		collisionShapeComboBox->SetEnabled(false);
		//		break;
		//	case 1:
		//		physicscomponent->rigidBody = true;
		//		//if (object->mesh != nullptr)
		//		//{
		//		//	object->mesh->softBody = false;
		//		//}

		//		kinematicCheckBox->SetEnabled(true);
		//		physicsTypeComboBox->SetEnabled(true);
		//		collisionShapeComboBox->SetEnabled(true);
		//		break;
		//	//case 2:
		//	//	object->rigidBody = false;
		//	//	if (object->mesh != nullptr)
		//	//	{
		//	//		object->mesh->softBody = true;
		//	//	}

		//	//	kinematicCheckBox->SetEnabled(false);
		//	//	physicsTypeComboBox->SetEnabled(true);
		//	//	collisionShapeComboBox->SetEnabled(false);
		//	//	break;
		//	default:
		//		assert(0);
		//		break;
		//	}

		//	wiRenderer::physicsEngine->registerObject(*physicscomponent);

		//}
	});
	simulationTypeComboBox->SetSelected(0);
	simulationTypeComboBox->SetEnabled(true);
	simulationTypeComboBox->SetTooltip("Set simulation type.");
	objectWindow->AddWidget(simulationTypeComboBox);

	kinematicCheckBox = new wiCheckBox("Kinematic: ");
	kinematicCheckBox->SetTooltip("Toggle kinematic behaviour.");
	kinematicCheckBox->SetPos(XMFLOAT2(x, y += 30));
	kinematicCheckBox->SetCheck(false);
	kinematicCheckBox->OnClick([&](wiEventArgs args) {
		//PhysicsComponent* physicscomponent = wiRenderer::GetScene().physicscomponents.GetComponent(entity);
		//if (physicscomponent != nullptr)
		//{
		//	wiRenderer::physicsEngine->removeObject(object);

		//	object->kinematic = args.bValue;

		//	wiRenderer::physicsEngine->registerObject(object);
		//}
	});
	objectWindow->AddWidget(kinematicCheckBox);

	physicsTypeComboBox = new wiComboBox("Contribution Type:");
	physicsTypeComboBox->SetSize(XMFLOAT2(100, 20));
	physicsTypeComboBox->SetPos(XMFLOAT2(x, y += 30));
	physicsTypeComboBox->AddItem("Active");
	physicsTypeComboBox->AddItem("Passive");
	physicsTypeComboBox->OnSelect([&](wiEventArgs args) {
		//if (object != nullptr)
		//{
		//	wiRenderer::physicsEngine->removeObject(object);

		//	switch (args.iValue)
		//	{
		//	case 0:
		//		object->physicsType = "ACTIVE";
		//		break;
		//	case 1:
		//		object->physicsType = "PASSIVE";
		//		break;
		//	default:
		//		break;
		//	}

		//	wiRenderer::physicsEngine->registerObject(object);
		//}
	});
	physicsTypeComboBox->SetSelected(0);
	physicsTypeComboBox->SetEnabled(true);
	physicsTypeComboBox->SetTooltip("Set physics type.");
	objectWindow->AddWidget(physicsTypeComboBox);

	collisionShapeComboBox = new wiComboBox("Collision Shape:");
	collisionShapeComboBox->SetSize(XMFLOAT2(100, 20));
	collisionShapeComboBox->SetPos(XMFLOAT2(x, y += 30));
	collisionShapeComboBox->AddItem("Box");
	collisionShapeComboBox->AddItem("Sphere");
	collisionShapeComboBox->AddItem("Capsule");
	collisionShapeComboBox->AddItem("Convex Hull");
	collisionShapeComboBox->AddItem("Triangle Mesh");
	collisionShapeComboBox->OnSelect([&](wiEventArgs args) {
		//if (object != nullptr)
		//{
		//	wiRenderer::physicsEngine->removeObject(object);

		//	switch (args.iValue)
		//	{
		//	case 0:
		//		object->collisionShape = "BOX";
		//		break;
		//	case 1:
		//		object->collisionShape = "SPHERE";
		//		break;
		//	case 2:
		//		object->collisionShape = "CAPSULE";
		//		break;
		//	case 3:
		//		object->collisionShape = "CONVEX_HULL";
		//		break;
		//	case 4:
		//		object->collisionShape = "MESH";
		//		break;
		//	default:
		//		break;
		//	}

		//	wiRenderer::physicsEngine->registerObject(object);
		//}
	});
	collisionShapeComboBox->SetSelected(0);
	collisionShapeComboBox->SetEnabled(true);
	collisionShapeComboBox->SetTooltip("Set rigid body collision shape.");
	objectWindow->AddWidget(collisionShapeComboBox);




	objectWindow->Translate(XMFLOAT3(1300, 100, 0));
	objectWindow->SetVisible(false);

	SetEntity(INVALID_ENTITY);
}


ObjectWindow::~ObjectWindow()
{
	objectWindow->RemoveWidgets(true);
	GUI->RemoveWidget(objectWindow);
	SAFE_DELETE(objectWindow);
}


void ObjectWindow::SetEntity(Entity entity)
{
	if (this->entity == entity)
		return;

	this->entity = entity;

	Scene& scene = wiRenderer::GetScene();

	const ObjectComponent* object = scene.objects.GetComponent(entity);

	if (object != nullptr)
	{
		renderableCheckBox->SetCheck(object->renderable);
		cascadeMaskSlider->SetValue((float)object->cascadeMask);
		ditherSlider->SetValue(object->GetTransparency());

		const RigidBodyPhysicsComponent* physicsComponent = scene.rigidbodies.GetComponent(entity);

		if (physicsComponent != nullptr)
		{
			//if (physicsComponent->rigidBody)
			//{
			//	simulationTypeComboBox->SetSelected(1);
			//}
			//else
			//{
			//	if (object->mesh != nullptr)
			//	{
			//		if (object->mesh->softBody)
			//		{
			//			simulationTypeComboBox->SetSelected(2);
			//		}
			//		else
			//		{
			//			simulationTypeComboBox->SetSelected(0);
			//		}
			//	}
			//	else
			//	{
			//		simulationTypeComboBox->SetSelected(0);
			//	}
			//}

			kinematicCheckBox->SetCheck(physicsComponent->kinematic);

			//if (!physicsComponent->physicsType.compare("ACTIVE"))
			//{
			//	physicsTypeComboBox->SetSelected(0);
			//}
			//else if (!physicsComponent->physicsType.compare("PASSIVE"))
			//{
			//	physicsTypeComboBox->SetSelected(1);
			//}

			if (physicsComponent->shape == RigidBodyPhysicsComponent::CollisionShape::BOX)
			{
				collisionShapeComboBox->SetSelected(0);
			}
			else if (physicsComponent->shape == RigidBodyPhysicsComponent::CollisionShape::SPHERE)
			{
				collisionShapeComboBox->SetSelected(1);
			}
			else if (physicsComponent->shape == RigidBodyPhysicsComponent::CollisionShape::CAPSULE)
			{
				collisionShapeComboBox->SetSelected(2);
			}
			else if (physicsComponent->shape == RigidBodyPhysicsComponent::CollisionShape::CONVEX_HULL)
			{
				collisionShapeComboBox->SetSelected(3);
			}
			else if (physicsComponent->shape == RigidBodyPhysicsComponent::CollisionShape::TRIANGLE_MESH)
			{
				collisionShapeComboBox->SetSelected(4);
			}
		}

		objectWindow->SetEnabled(true);


		switch (simulationTypeComboBox->GetSelected())
		{
		case 1:
			kinematicCheckBox->SetEnabled(true);
			physicsTypeComboBox->SetEnabled(true);
			collisionShapeComboBox->SetEnabled(true);
			break;
		case 2:
			kinematicCheckBox->SetEnabled(false);
			physicsTypeComboBox->SetEnabled(true);
			collisionShapeComboBox->SetEnabled(false);
			break;
		default:
			kinematicCheckBox->SetEnabled(false);
			physicsTypeComboBox->SetEnabled(false);
			collisionShapeComboBox->SetEnabled(false);
			break;
		}
	}
	else
	{
		objectWindow->SetEnabled(false);
	}

}
