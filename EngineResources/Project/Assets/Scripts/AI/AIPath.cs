using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole;

public class AIPath {

	public TheGameObject PathGroup = null;
	public bool IsPatrol = false;
	
	TheGameObject Self = TheGameObject.Self;
	
	List<TheGameObject> Nodes = new List<TheGameObject>();
	List<TheVector3> NodePosition = new List<TheVector3>();
	
	TheVector3 next_node;
	
	void Start () {
		TheGameObject[] nodes = PathGroup.GetAllChilds();
		foreach (TheGameObject node in nodes)
		{
			Nodes.Add(node);
			NodePosition.Add(node.GetComponent<TheTransform>().GlobalPosition);
			//TheConsole.Log("Name: "+ node.name);
		}
		//TheConsole.Log("Total Nodes: "+Nodes.Count);
	}
	
	void Update () {
		
		
		
	}
}