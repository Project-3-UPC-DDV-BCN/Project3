using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole;

public class AIPath {

	List<TheGameObject> Nodes = new List<TheGameObject>();
	List<TheTransform> NodeTransform = new List<TheTransform>();
	public string node_tag;
	public bool IsPatrol = false;
	
	void Start () {
		int i = 0;
		TheGameObject[] nodes = TheGameObject.GetGameObjectsWithTag(node_tag);
		foreach (TheGameObject node in nodes)
		{
			Nodes.Add(node);
			NodeTransform.Add(node.GetComponent<TheTransform>());
		}
		TheConsole.Log("Total Nodes: "+Nodes.Count);
	}
	
	void Update () {
		
	}
}