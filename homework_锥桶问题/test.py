import torch
from torchvision import transforms, datasets
from torch.utils.data import DataLoader
from net import mixed_net, is_valid_image

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
CLASS_NAMES = ["blue", "red", "yellow"]


def evaluate(name, model, test_loader, class_to_idx):
    idx_to_class = {v: k for k, v in class_to_idx.items()}
    class_correct = {c: 0 for c in class_to_idx}
    class_total = {c: 0 for c in class_to_idx}
    correct = 0
    total = 0

    model.eval()
    with torch.no_grad():
        for datas, labels in test_loader:
            datas = datas.to(device)
            labels = labels.to(device)
            outputs = model(datas)
            pred = outputs.argmax(dim=1)
            correct += (pred == labels).sum().item()
            total += labels.size(0)
            for i in range(labels.size(0)):
                cname = idx_to_class[labels[i].item()]
                class_total[cname] += 1
                if pred[i] == labels[i]:
                    class_correct[cname] += 1

    acc = 100.0 * correct / total
    print(f"{name} Overall Accuracy: {acc:.3f}%")
    parts = []
    for cname in CLASS_NAMES:
        if class_total[cname] > 0:
            ca = 100.0 * class_correct[cname] / class_total[cname]
            parts.append(f"{cname} : {ca:.3f}%")
    print(f"Category Accuracy : {'  '.join(parts)}")


if __name__ == "__main__":
    transform = transforms.Compose([
        transforms.Resize([64, 64]),
        transforms.ToTensor(),
        transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5)),
    ])

    BATCH_SIZE = 64
    testset1 = datasets.ImageFolder(
        root=r"dataset/test1",
        transform=transform,
        is_valid_file=is_valid_image,
    )
    testset2 = datasets.ImageFolder(
        root=r"dataset/test2",
        transform=transform,
        is_valid_file=is_valid_image,
    )
    test_loader1 = DataLoader(testset1, batch_size=BATCH_SIZE, shuffle=False)
    test_loader2 = DataLoader(testset2, batch_size=BATCH_SIZE, shuffle=False)

    model_path = r"pth/model_best.pth"
    model = mixed_net().to(device)
    model.load_state_dict(torch.load(model_path, map_location=device))

    evaluate("test", model, test_loader1, testset1.class_to_idx)
    evaluate("test2", model, test_loader2, testset2.class_to_idx)
